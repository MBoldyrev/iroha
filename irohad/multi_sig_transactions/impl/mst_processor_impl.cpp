/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "multi_sig_transactions/mst_processor_impl.hpp"

#include <utility>

#include <rxcpp/operators/rx-filter.hpp>
#include <rxcpp/operators/rx-map.hpp>
#include <rxcpp/operators/rx-take.hpp>
#include "logger/logger.hpp"

using shared_model::interface::types::PublicKeyHexStringView;

namespace iroha {

  FairMstProcessor::FairMstProcessor(
      std::shared_ptr<network::MstTransport> transport,
      std::shared_ptr<MstStorage> storage,
      std::shared_ptr<PropagationStrategy> strategy,
      std::shared_ptr<MstTimeProvider> time_provider,
      logger::LoggerPtr log)
      : MstProcessor(log),  // use the same logger in base class
        log_(std::move(log)),
        transport_(std::move(transport)),
        storage_(std::move(storage)),
        strategy_(std::move(strategy)),
        time_provider_(std::move(time_provider)),
        propagation_subscriber_(strategy_->emitter().subscribe(
            [log_wp = std::weak_ptr<logger::Logger>{log_},
             transport_wp = std::weak_ptr<network::MstTransport>{transport_},
             storage_wp = std::weak_ptr<MstStorage>{storage_},
             time_provider_wp = std::weak_ptr<MstTimeProvider>{time_provider_}](
                PropagationStrategy::PropagationData const &data) {
              auto log = log_wp.lock();
              auto transport = transport_wp.lock();
              auto storage = storage_wp.lock();
              auto time_provider = time_provider_wp.lock();

              if (log and transport and storage and time_provider) {
                auto const current_time = time_provider->getCurrentTime();
                auto const peers_number = data.size();

                for (auto const &dst_peer : data) {
                  auto diff = storage->getDiffState(
                      PublicKeyHexStringView{dst_peer->pubkey()}, current_time);
                  if (not diff.isEmpty()) {
                    log->info("Propagate new data[{}]", peers_number);
                    transport->sendState(dst_peer, diff)
                        .take(1)
                        .filter([](auto is_ok) { return is_ok; })
                        .subscribe([storage,
                                    dst_peer = std::move(dst_peer),
                                    diff = std::move(diff)](auto) {
                          storage->apply(
                              PublicKeyHexStringView{dst_peer->pubkey()}, diff);
                        });
                  }
                }
              }
            })) {}

  FairMstProcessor::~FairMstProcessor() {
    propagation_subscriber_.unsubscribe();
  }

  // -------------------------| MstProcessor override |-------------------------

  auto FairMstProcessor::propagateBatchImpl(const iroha::DataType &batch)
      -> decltype(propagateBatch(batch)) {
    auto state_update = storage_->updateOwnState(batch);
    completedBatchesNotify(*state_update.completed_state_);
    updatedBatchesNotify(*state_update.updated_state_);
    expiredBatchesNotify(
        storage_->extractExpiredTransactions(time_provider_->getCurrentTime()));
  }

  auto FairMstProcessor::onStateUpdateImpl() const
      -> decltype(onStateUpdate()) {
    return state_subject_.get_observable();
  }

  auto FairMstProcessor::onPreparedBatchesImpl() const
      -> decltype(onPreparedBatches()) {
    return batches_subject_.get_observable();
  }

  auto FairMstProcessor::onExpiredBatchesImpl() const
      -> decltype(onExpiredBatches()) {
    return expired_subject_.get_observable();
  }

  bool FairMstProcessor::batchInStorageImpl(const DataType &batch) const {
    return storage_->batchInStorage(batch);
  }

  // -------------------| MstTransportNotification override |-------------------

  void FairMstProcessor::onNewState(PublicKeyHexStringView from,
                                    MstState &&new_state) {
    log_->info("Applying new state");
    auto current_time = time_provider_->getCurrentTime();

    // no need to add already expired batches to local state
    new_state.eraseExpired(current_time);
    auto state_update = storage_->apply(from, new_state);

    // updated batches
    updatedBatchesNotify(*state_update.updated_state_);
    log_->info("New batches size: {}",
               state_update.updated_state_->getBatches().size());

    // completed batches
    completedBatchesNotify(*state_update.completed_state_);

    // expired batches
    // not nesessary to do it right here, just use the occasion to clean storage
    expiredBatchesNotify(storage_->extractExpiredTransactions(current_time));
  }

  // -----------------------------| private api |-----------------------------

  // TODO [IR-1687] Akvinikym 10.09.18: three methods below should be one
  void FairMstProcessor::completedBatchesNotify(ConstRefState state) const {
    if (not state.isEmpty()) {
      state.iterateBatches([this](const auto &batch) {
        batches_subject_.get_subscriber().on_next(batch);
      });
    }
  }

  void FairMstProcessor::updatedBatchesNotify(ConstRefState state) const {
    if (not state.isEmpty()) {
      state_subject_.get_subscriber().on_next(
          std::make_shared<MstState>(state));
    }
  }

  void FairMstProcessor::expiredBatchesNotify(ConstRefState state) const {
    if (not state.isEmpty()) {
      state.iterateBatches([this](const auto &batch) {
        expired_subject_.get_subscriber().on_next(batch);
      });
    }
  }

}  // namespace iroha
