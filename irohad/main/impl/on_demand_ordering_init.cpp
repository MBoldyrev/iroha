/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "main/impl/on_demand_ordering_init.hpp"

#include <random>
#include <vector>

#include <boost/algorithm/string/join.hpp>
#include <boost/range/algorithm/transform.hpp>
#include "common/bind.hpp"
#include "common/delay.hpp"
#include "cryptography/crypto_provider/crypto_defaults.hpp"
#include "datetime/time.hpp"
#include "interfaces/common_objects/peer.hpp"
#include "interfaces/common_objects/types.hpp"
#include "logger/logger.hpp"
#include "logger/logger_manager.hpp"
#include "ordering/impl/on_demand_common.hpp"
#include "ordering/impl/on_demand_connection_manager.hpp"
#include "ordering/impl/on_demand_ordering_gate.hpp"
#include "ordering/impl/on_demand_ordering_service_impl.hpp"
#include "ordering/impl/on_demand_os_client_grpc.hpp"
#include "ordering/impl/on_demand_os_server_grpc.hpp"
#include "ordering/impl/ordering_gate_cache/on_demand_cache.hpp"

namespace {
  /// The distance between two rounds, from which the first one provides a state
  /// (hash) that is used to determine the ordering service at the second
  constexpr size_t kHashDelay = 2;

  /// match event and call corresponding lambda depending on sync_outcome
  template <typename OnBlocks, typename OnNothing>
  auto matchEvent(const iroha::synchronizer::SynchronizationEvent &event,
                  OnBlocks &&on_blocks,
                  OnNothing &&on_nothing) {
    using iroha::synchronizer::SynchronizationOutcomeType;
    switch (event.sync_outcome) {
      case SynchronizationOutcomeType::kCommit:
        return std::forward<OnBlocks>(on_blocks)(event);
      case SynchronizationOutcomeType::kReject:
      case SynchronizationOutcomeType::kNothing:
        return std::forward<OnNothing>(on_nothing)(event);
      default:
        BOOST_ASSERT_MSG(false, "Unknown value");
    }
  }
}  // namespace

namespace iroha {
  namespace network {

    OnDemandOrderingInit::OnDemandOrderingInit(logger::LoggerPtr log)
        : log_(std::move(log)) {}

    auto OnDemandOrderingInit::createNotificationFactory(
        std::shared_ptr<network::AsyncGrpcClient<google::protobuf::Empty>>
            async_call,
        std::shared_ptr<TransportFactoryType> proposal_transport_factory,
        std::chrono::milliseconds delay,
        const logger::LoggerManagerTreePtr &ordering_log_manager) {
      return std::make_shared<ordering::transport::OnDemandOsClientGrpcFactory>(
          std::move(async_call),
          std::move(proposal_transport_factory),
          [] { return std::chrono::system_clock::now(); },
          delay,
          ordering_log_manager->getChild("NetworkClient")->getLogger());
    }

    auto OnDemandOrderingInit::createConnectionManager(
        std::shared_ptr<ametsuchi::PeerQueryFactory> peer_query_factory,
        std::shared_ptr<network::AsyncGrpcClient<google::protobuf::Empty>>
            async_call,
        std::shared_ptr<TransportFactoryType> proposal_transport_factory,
        std::chrono::milliseconds delay,
        std::vector<shared_model::interface::types::HashType> initial_hashes,
        const logger::LoggerManagerTreePtr &ordering_log_manager) {
      // Since top block will be the first in notifier observable, the required
      // amount of hashes of previous blocks are prepended.
      const auto hash_delay = initial_hashes.size();

      // Emit hashes from committed blocks shifted by initial_hashes:
      auto delayed_hashes =
          rxcpp::observable<>::iterate(initial_hashes)
              .concat(notifier.get_observable().flat_map([](auto commit) {
                return commit.synced_blocks.map(
                    [](auto block) { return block->hash(); });
              }));

      auto map_next_peer = [this, peer_query_factory](auto &&latest_data)
          -> ordering::OnDemandConnectionManager::CurrentPeers {
        auto &latest_commit = std::get<0>(latest_data);
        auto &current_hashes = std::get<1>(latest_data);

        consensus::Round current_round = latest_commit.round;

        std::vector<std::shared_ptr<shared_model::interface::Peer>>
            current_peers;

        auto update_peers = [&peer_query_factory, &current_peers] {
          current_peers.clear();
          // retrieve peer list from database
          // TODO lebdron 08.11.2018 IR-1853 Refactor PeerQuery without
          // database access and optional
          peer_query_factory->createPeerQuery() | [](auto &&query) {
            return query->getLedgerPeers();
          } | [&current_peers](auto &&peers) {
            current_peers = std::move(peers);
          };
          assert(not current_peers.empty());
        };
        update_peers();

        using Permutation = std::vector<size_t>;
        struct Ordering {
          size_t next() {
            auto val = permutation[current_index];
            current_index = (current_index + 1) % permutation.size();
          }
          Permutation permutation;
          size_t current_index;
        };
        Ordering current_ordering;

        // generate ordering of peers list for current block hashes
        auto update_ordering = [&]() {
          log_->debug(
              "Using hashes: {}",
              boost::algorithm::join(
                  current_hashes | boost::transform([](const auto &hash) {
                    return hash.toString();
                  }),
                  "', '"));

          std::vector<decltype(current_hashes().front().hash.blob().front())>
              seed;
          seed.reserve(current_hashes.size()
                       * current_hashes().front().blob().size());
          for (const auto &hash : current_hashes) {
            std::copy(hash.blob().cbegin(),
                      hash.blob().cend(),
                      std::back_inserter(seed));
          }
          std::seed_seq sseed{seed.begin(), seed.end()};

          // TODO andrei 08.11.2018 IR-1850 Refactor default_random_engine
          // usages with platform-independent class
          std::default_random_engine gen;
          gen.seed(sseed);

          current_ordering.index = 0;
          auto &permutation = current_ordering.permutation;
          permutation.resize(current_peers.size());
          std::iota(permutation.begin(), permutation.end(), 0);

          std::shuffle(permutation.begin(), permutation.end(), gen);
        };

        update_ordering();

        auto on_blocks = [this,
                          peer_query_factory,
                          current_hashes,
                          &current_round,
                          &update_ordering](const auto &commit) {
          current_round = ordering::nextCommitRound(current_round);
          update_peers();
          update_ordering();
        };
        auto on_nothing = [&current_round](const auto &) {
          current_round = ordering::nextRejectRound(current_round);
        };

        matchEvent(latest_commit, on_blocks, on_nothing);

        return current_peers[current_ordering.next()];
      };

      auto next_peer = notifier.get_observable()
                           .with_latest_from(delayed_hashes)
                           .map(map_next_peer);

      auto current_peers =
          next_peers.zip(next_peers, next_peers.skip(hash_delay))
              .map([](auto &peers) {
                using ODCM = ordering::OnDemandConnectionManager;
                ODCM::CurrentPeers cur_peers;
                cur_peers.peers.at(ODCM::kIssuer) = std::get<0>(peers);
                cur_peers.peers.at(ODCM::kConsumer) = std::get<1>(peers);
                return peers;
              });

      return std::make_shared<ordering::OnDemandConnectionManager>(
          createNotificationFactory(std::move(async_call),
                                    std::move(proposal_transport_factory),
                                    delay,
                                    ordering_log_manager),
          current_peers,
          ordering_log_manager->getChild("ConnectionManager")->getLogger());
    }

    auto OnDemandOrderingInit::createGate(
        std::shared_ptr<ordering::OnDemandOrderingService> ordering_service,
        std::shared_ptr<ordering::transport::OdOsNotification> network_client,
        std::shared_ptr<ordering::cache::OrderingGateCache> cache,
        std::shared_ptr<shared_model::interface::UnsafeProposalFactory>
            proposal_factory,
        std::shared_ptr<ametsuchi::TxPresenceCache> tx_cache,
        std::function<std::chrono::milliseconds(
            const synchronizer::SynchronizationEvent &)> delay_func,
        size_t max_number_of_transactions,
        const logger::LoggerManagerTreePtr &ordering_log_manager) {
      auto map = [](auto commit) {
        return matchEvent(
            commit,
            [](const auto &commit)
                -> ordering::OnDemandOrderingGate::BlockRoundEventType {
              ordering::cache::OrderingGateCache::HashesSetType hashes;
              commit.synced_blocks.as_blocking().subscribe(
                  [&hashes](const auto &block) {
                    const auto &committed = block->transactions();
                    std::transform(committed.begin(),
                                   committed.end(),
                                   std::inserter(hashes, hashes.end()),
                                   [](const auto &transaction) {
                                     return transaction.hash();
                                   });
                    const auto &rejected =
                        block->rejected_transactions_hashes();
                    std::copy(rejected.begin(),
                              rejected.end(),
                              std::inserter(hashes, hashes.end()));
                  });
              return ordering::OnDemandOrderingGate::BlockEvent{
                  ordering::nextCommitRound(commit.round), hashes};
            },
            [](const auto &nothing)
                -> ordering::OnDemandOrderingGate::BlockRoundEventType {
              return ordering::OnDemandOrderingGate::EmptyEvent{
                  ordering::nextRejectRound(nothing.round)};
            });
      };

      return std::make_shared<ordering::OnDemandOrderingGate>(
          std::move(ordering_service),
          std::move(network_client),
          notifier.get_observable()
              .lift<iroha::synchronizer::SynchronizationEvent>(
                  iroha::makeDelay<iroha::synchronizer::SynchronizationEvent>(
                      delay_func, rxcpp::identity_current_thread()))
              .map(map),
          std::move(cache),
          std::move(proposal_factory),
          std::move(tx_cache),
          max_number_of_transactions,
          ordering_log_manager->getChild("Gate")->getLogger());
    }

    auto OnDemandOrderingInit::createService(
        size_t max_number_of_transactions,
        std::shared_ptr<shared_model::interface::UnsafeProposalFactory>
            proposal_factory,
        std::shared_ptr<ametsuchi::TxPresenceCache> tx_cache,
        const logger::LoggerManagerTreePtr &ordering_log_manager) {
      return std::make_shared<ordering::OnDemandOrderingServiceImpl>(
          max_number_of_transactions,
          std::move(proposal_factory),
          std::move(tx_cache),
          ordering_log_manager->getChild("Service")->getLogger());
    }

    OnDemandOrderingInit::~OnDemandOrderingInit() {
      notifier.get_subscriber().unsubscribe();
    }

    std::shared_ptr<iroha::network::OrderingGate>
    OnDemandOrderingInit::initOrderingGate(
        size_t max_number_of_transactions,
        std::chrono::milliseconds delay,
        std::vector<shared_model::interface::types::HashType> initial_hashes,
        std::shared_ptr<ametsuchi::PeerQueryFactory> peer_query_factory,
        std::shared_ptr<
            ordering::transport::OnDemandOsServerGrpc::TransportFactoryType>
            transaction_factory,
        std::shared_ptr<shared_model::interface::TransactionBatchParser>
            batch_parser,
        std::shared_ptr<shared_model::interface::TransactionBatchFactory>
            transaction_batch_factory,
        std::shared_ptr<network::AsyncGrpcClient<google::protobuf::Empty>>
            async_call,
        std::shared_ptr<shared_model::interface::UnsafeProposalFactory>
            proposal_factory,
        std::shared_ptr<TransportFactoryType> proposal_transport_factory,
        std::shared_ptr<ametsuchi::TxPresenceCache> tx_cache,
        std::function<std::chrono::milliseconds(
            const synchronizer::SynchronizationEvent &)> delay_func,
        logger::LoggerManagerTreePtr ordering_log_manager) {
      auto ordering_service = createService(max_number_of_transactions,
                                            proposal_factory,
                                            tx_cache,
                                            ordering_log_manager);
      service = std::make_shared<ordering::transport::OnDemandOsServerGrpc>(
          ordering_service,
          std::move(transaction_factory),
          std::move(batch_parser),
          std::move(transaction_batch_factory),
          ordering_log_manager->getChild("Server")->getLogger());
      return createGate(
          ordering_service,
          createConnectionManager(std::move(peer_query_factory),
                                  std::move(async_call),
                                  std::move(proposal_transport_factory),
                                  delay,
                                  std::move(initial_hashes),
                                  ordering_log_manager),
          std::make_shared<ordering::cache::OnDemandCache>(),
          std::move(proposal_factory),
          std::move(tx_cache),
          std::move(delay_func),
          max_number_of_transactions,
          ordering_log_manager);
    }

  }  // namespace network
}  // namespace iroha
