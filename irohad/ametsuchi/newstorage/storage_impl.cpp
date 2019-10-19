/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ametsuchi/newstorage/storage_impl.hpp"

#include <utility>

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <boost/range/algorithm/replace_if.hpp>
#include "ametsuchi/newstorage/mutable_storage_impl.hpp"
#include "ametsuchi/newstorage/peer_query_wsv.hpp"
//#include "newstorage/impl/postgres_block_index.hpp"
#include "ametsuchi/newstorage/block_query_impl.hpp"
//#include "newstorage/impl/postgres_block_storage_factory.hpp"
//#include "newstorage/impl/postgres_command_executor.hpp"
//#include "newstorage/impl/postgres_indexer.hpp"
#include "ametsuchi/newstorage/query_executor_impl.hpp"
#include "ametsuchi/newstorage/specific_query_executor_impl.hpp"
//#include "newstorage/impl/postgres_wsv_command.hpp"
#include "ametsuchi/newstorage/wsv_query_impl.hpp"
#include "ametsuchi/newstorage/temporary_wsv_impl.hpp"
//#include "newstorage/tx_executor.hpp"
#include "backend/protobuf/permissions.hpp"
#include "common/bind.hpp"
#include "common/byteutils.hpp"
#include "cryptography/public_key.hpp"
#include "logger/logger.hpp"
#include "logger/logger_manager.hpp"

namespace iroha {
  namespace newstorage {

/*
    StorageImpl::StorageImpl(

          /*
       * std::string path;
        std::shared_ptr<shared_model::interface::PermissionToString>
            perm_converter;
        std::shared_ptr<PendingTransactionStorage> pending_txs_storage;
        std::shared_ptr<shared_model::interface::QueryResponseFactory>
            query_response_factory;
        logger::LoggerManagerTreePtr log_manager;
        std::shared_ptr<shared_model::interface::Block>
            genesis_block;
        std::vector<shared_model::interface::Peer> alternative_peers;
        bool overwrite_ledger = false;
        bool reset_wsv = false;
       */

        const std::string& location,
        std::shared_ptr<shared_model::interface::PermissionToString>
        perm_converter,
        std::shared_ptr<PendingTransactionStorage> pending_txs_storage,
        std::shared_ptr<shared_model::interface::QueryResponseFactory>
        query_response_factory,
        logger::LoggerManagerTreePtr log_manager)
        : block_store_(std::move(block_store)),
          notifier_(notifier_lifetime_),
          perm_converter_(std::move(perm_converter)),
          pending_txs_storage_(std::move(pending_txs_storage)),
          query_response_factory_(std::move(query_response_factory)),
          temporary_block_storage_factory_(
              std::move(temporary_block_storage_factory)),
          log_manager_(std::move(log_manager)),
          log_(log_manager_->getLogger()),
          prepared_blocks_enabled_(false),
          block_is_prepared_(false),
          db_({ "xxxx"}, nullptr)
    {
      // TODO db init
      // TODO ledger state init
    }

    expected::Result<std::shared_ptr<StorageImpl>, std::string>
    StorageImpl::create(
        std::shared_ptr<shared_model::interface::PermissionToString>
          perm_converter,
        std::shared_ptr<PendingTransactionStorage> pending_txs_storage,
        std::shared_ptr<shared_model::interface::QueryResponseFactory>
          query_response_factory,
        std::unique_ptr<ametsuchi::BlockStorageFactory> temporary_block_storage_factory,
        std::unique_ptr<ametsuchi::BlockStorage> persistent_block_storage,
        logger::LoggerManagerTreePtr log_manager)
    {
      return expected::makeValue(std::shared_ptr<StorageImpl>(
          new StorageImpl(std::move(persistent_block_storage),
                          perm_converter,
                          std::move(pending_txs_storage),
                          std::move(query_response_factory),
                          std::move(temporary_block_storage_factory),
                          std::move(log_manager))));
    }
*/

    std::unique_ptr<ametsuchi::TemporaryWsv> StorageImpl::createTemporaryWsv() {
      return std::make_unique<TemporaryWsvImpl>(
          mutable_wsv_,
          log_manager_->getChild("TemporaryWorldStateView"));
    }

    std::unique_ptr<ametsuchi::MutableStorage> StorageImpl::createMutableStorage() {
      return createMutableStorage(*temporary_block_storage_factory_);
    }

    std::unique_ptr<ametsuchi::MutableStorage> StorageImpl::createMutableStorage(
        ametsuchi::BlockStorageFactory &storage_factory) {

      return std::make_unique<MutableStorageImpl>(
          ledger_state_,
          mutable_wsv_,
          storage_factory.create(),
          log_manager_->getChild("MutableStorageImpl"));
    }

    boost::optional<std::shared_ptr<ametsuchi::PeerQuery>> StorageImpl::createPeerQuery()
        const {
      auto wsv = getWsvQuery();
      if (not wsv) {
        return boost::none;
      }
      return boost::make_optional<std::shared_ptr<ametsuchi::PeerQuery>>(
          std::make_shared<PeerQueryWsv>(wsv));
    }

    boost::optional<std::shared_ptr<ametsuchi::BlockQuery>> StorageImpl::createBlockQuery()
        const {
      auto block_query = getBlockQuery();
      if (not block_query) {
        return boost::none;
      }
      return boost::make_optional(block_query);
    }

    std::shared_ptr<ametsuchi::WsvQuery> StorageImpl::getWsvQuery() const {
      return std::make_shared<WsvQueryImpl>(
          immutable_wsv_,
          log_manager_->getChild("WsvQuery")->getLogger());
    }

    std::shared_ptr<ametsuchi::BlockQuery> StorageImpl::getBlockQuery() const {
      return std::make_shared<BlockQueryImpl>(
          index_db_,
          *block_store_,
          log_manager_->getChild("BlockQuery")->getLogger());
    }

    boost::optional<std::shared_ptr<ametsuchi::QueryExecutor>>
    StorageImpl::createQueryExecutor(
        std::shared_ptr<PendingTransactionStorage> pending_txs_storage,
        std::shared_ptr<shared_model::interface::QueryResponseFactory>
            response_factory) const {
      auto log_manager = log_manager_->getChild("QueryExecutor");
      return boost::make_optional<std::shared_ptr<ametsuchi::QueryExecutor>>(
          std::make_shared<QueryExecutorImpl>(
              immutable_wsv_,
              response_factory,
              std::make_shared<SpecificQueryExecutorImpl>(
                  immutable_wsv_,
                  *block_store_,
                  std::move(pending_txs_storage),
                  response_factory,
                  perm_converter_,
                  log_manager->getChild("SpecificQueryExecutor")->getLogger()),
              log_manager->getLogger()));
    }

    bool StorageImpl::insertBlock(
        std::shared_ptr<const shared_model::interface::Block> block) {
      log_->info("create mutable storage");
      auto mutable_storage = createMutableStorage();
      bool is_inserted = mutable_storage->apply(block);
      log_->info("Block {}inserted", is_inserted ? "" : "not ");
      commit(std::move(mutable_storage));
      return is_inserted;
    }


    /*
    expected::Result<void, std::string> StorageImpl::insertPeer(
        const shared_model::interface::Peer &peer) {
      log_->info("Insert peer {}", peer.pubkey().hex());
      try {
        db_.insertPeer(peer.address(), peer.pubkey().hex());
      } catch (const std::exception& e) {
        return expected::makeError(e.what());
      }
      return {};
    }

    void StorageImpl::resetPeers() {
      log_->info("Remove everything from peers table");
      try {
        db_.dropPeers();
      } catch (const std::exception& e) {
        log_->error(e.what());
      }
    }

    void StorageImpl::dropStorage() {
      log_->info("drop storage");

      // erase blocks
      log_->info("drop block store");
      block_store_->clear();

      log_->info("Dropping database");
      try {
        db_.dropAll();
      } catch (const std::exception& e) {
        log_->error(e.what());
      }
    }
*/
    void StorageImpl::freeConnections() {}

    StorageImpl::~StorageImpl() {
      notifier_lifetime_.unsubscribe();
      freeConnections();
    }

    StorageImpl::StoreBlockResult StorageImpl::storeBlock(
        std::shared_ptr<const shared_model::interface::Block> block) {
      if (block_store_->insert(block)) {
        notifier_.get_subscriber().on_next(block);
        return {};
      }
      return expected::makeError("Block insertion to storage failed");
    }

    void StorageImpl::reset() {
      resetWsv().match(
          [this](auto &&v) {
            log_->debug("drop blocks from disk");
            block_store_->clear();
          },
          [this](auto &&e) {
            log_->warn("Failed to drop WSV. Reason: {}", e.error);
          });
    }
/*
    expected::Result<void, std::string> StorageImpl::resetWsv() {
      log_->debug("drop wsv records from db tables");
      try {
        // rollback possible prepared transaction
        // TODO tryRollback(sql);
        db_.dropAll();
      } catch (std::exception &e) {
        return expected::makeError(e.what());
      }
      return {};
    }
*/
    rxcpp::observable<std::shared_ptr<const shared_model::interface::Block>>
    StorageImpl::on_commit() {
      return notifier_.get_observable();
    }


/*





    CommitResult StorageImpl::commit(
        std::unique_ptr<MutableStorage> mutable_storage) {
      auto storage = static_cast<MutableStorageImpl *>(mutable_storage.get());

      try {
        storage->sql_ << "COMMIT";
      } catch (std::exception &e) {
        storage->committed = false;
        return expected::makeError(e.what());
      }
      storage->committed = true;

      storage->block_storage_->forEach(
          [this](const auto &block) { this->storeBlock(block); });

      ledger_state_ = storage->getLedgerState();
      if (ledger_state_) {
        return expected::makeValue(ledger_state_.value());
      } else {
        return expected::makeError(
            "This should never happen - a missing ledger state after a "
            "successful commit!");
      }
    }

    bool StorageImpl::preparedCommitEnabled() const {
      return prepared_blocks_enabled_ and block_is_prepared_;
    }

    CommitResult StorageImpl::commitPrepared(
        std::shared_ptr<const shared_model::interface::Block> block) {
      if (not prepared_blocks_enabled_) {
        return expected::makeError(
            std::string{"prepared blocks are not enabled"});
      }

      if (not block_is_prepared_) {
        return expected::makeError("there are no prepared blocks");
      }

      log_->info("applying prepared block");

      try {
        std::shared_lock<std::shared_timed_mutex> lock(drop_mutex_);
        if (not connection_) {
          std::string msg(
              "commitPrepared: connection to database is not initialised");
          return expected::makeError(std::move(msg));
        }
        soci::session sql(*connection_);
        sql << "COMMIT PREPARED '" + prepared_block_name_ + "';";
        PostgresBlockIndex block_index(
            std::make_unique<PostgresIndexer>(sql),
            log_manager_->getChild("BlockIndex")->getLogger());
        block_index.index(*block);
        block_is_prepared_ = false;

        return storeBlock(block) | [this, &sql, &block]() -> CommitResult {
          decltype(
              std::declval<PostgresWsvQuery>().getPeers()) opt_ledger_peers;
          {
            auto peer_query = PostgresWsvQuery(
                sql, this->log_manager_->getChild("WsvQuery")->getLogger());
            if (not(opt_ledger_peers = peer_query.getPeers())) {
              return expected::makeError(
                  std::string{"Failed to get ledger peers! Will retry."});
            }
          }
          assert(opt_ledger_peers);

          ledger_state_ = std::make_shared<const LedgerState>(
              std::move(*opt_ledger_peers), block->height(), block->hash());
          return expected::makeValue(ledger_state_.value());
        };
      } catch (const std::exception &e) {
        std::string msg((boost::format("failed to apply prepared block %s: %s")
                         % block->hash().hex() % e.what())
                            .str());
        return expected::makeError(msg);
      }
    }





    void StorageImpl::prepareBlock(std::unique_ptr<TemporaryWsv> wsv) {
      auto &wsv_impl = static_cast<TemporaryWsvImpl &>(*wsv);
      if (not prepared_blocks_enabled_) {
        log_->warn("prepared blocks are not enabled");
        return;
      }
      if (block_is_prepared_) {
        log_->warn(
            "Refusing to add new prepared state, because there already is one. "
            "Multiple prepared states are not yet supported.");
      } else {
        soci::session &sql = wsv_impl.sql_;
        try {
          sql << "PREPARE TRANSACTION '" + prepared_block_name_ + "';";
          block_is_prepared_ = true;
        } catch (const std::exception &e) {
          log_->warn("failed to prepare state: {}", e.what());
        }

        log_->info("state prepared successfully");
      }
    }


    */
  }  // namespace newstorage
}  // namespace iroha
