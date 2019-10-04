/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ametsuchi/newstorage/mutable_storage_impl.hpp"
#include <boost/variant/apply_visitor.hpp>
#include "ametsuchi/command_executor.hpp"
#include "ametsuchi/ledger_state.hpp"
#include "ametsuchi/newstorage/block_index.hpp"
#include "ametsuchi/newstorage/block_index_impl.hpp"
#include "ametsuchi/newstorage/indexer_impl.hpp"
#include "ametsuchi/newstorage/mutable_wsv.hpp"
#include "ametsuchi/newstorage/wsv_query_impl.hpp"
#include "ametsuchi/tx_executor.hpp"
#include "interfaces/iroha_internal/block.hpp"
#include "logger/logger.hpp"
#include "logger/logger_manager.hpp"

namespace iroha {
  namespace newstorage {

    MutableStorageImpl::MutableStorageImpl(
        boost::optional<std::shared_ptr<const iroha::LedgerState>> ledger_state,
        std::shared_ptr<ametsuchi::CommandExecutor> command_executor,
        MutableWsv& db,
        std::shared_ptr<BlockIndexDB> index_db,
        std::unique_ptr<ametsuchi::BlockStorage> block_storage,
        logger::LoggerManagerTreePtr log_manager)
        : ledger_state_(std::move(ledger_state)),
          db_(db),
          log_(log_manager->getLogger()),
          block_storage_(std::move(block_storage))
    {
      peer_query_ = std::make_unique<WsvQueryImpl>(
              db_, log_manager->getChild("WsvQuery")->getLogger());

      block_index_ = std::make_unique<BlockIndexImpl>(
              std::make_unique<IndexerImpl>(std::move(index_db)),
              log_manager->getChild("BlockIndex")->getLogger());

      transaction_executor_ = std::make_unique<ametsuchi::TransactionExecutor>(
          std::move(command_executor));

      db_tx_ = db_.createTx();
    }

    bool MutableStorageImpl::apply(
        std::shared_ptr<const shared_model::interface::Block> block,
        MutableStoragePredicate predicate) {
      auto execute_transaction = [this](auto &transaction) -> bool {
        return expected::hasValue(
            transaction_executor_->execute(transaction, false));
      };

      log_->info("Applying block: height {}, hash {}",
                 block->height(),
                 block->hash().hex());

      auto block_applied =
          (not ledger_state_ or predicate(block, *ledger_state_.value()))
          and std::all_of(block->transactions().begin(),
                          block->transactions().end(),
                          execute_transaction);
      if (block_applied) {
        block_storage_->insert(block);
        block_index_->index(*block);

        auto opt_ledger_peers = peer_query_->getPeers();
        if (not opt_ledger_peers) {
          log_->error("Failed to get ledger peers!");
          return false;
        }

        ledger_state_ = std::make_shared<const LedgerState>(
            std::move(*opt_ledger_peers), block->height(), block->hash());
      }

      return block_applied;
    }

    template <typename Function>
    bool MutableStorageImpl::withSavepoint(Function &&function) {
      try {
        auto savepoint = db_.createSavepoint("savepoint_");

        auto function_executed = std::forward<Function>(function)();

        if (function_executed) {
          savepoint.release();
        }
        return function_executed;

      } catch (std::exception &e) {
        log_->warn("Apply has failed. Reason: {}", e.what());
      }
      return false;
    }

    bool MutableStorageImpl::apply(
        std::shared_ptr<const shared_model::interface::Block> block) {
      return withSavepoint([&] {
        return this->apply(block, [](const auto &, auto &) { return true; });
      });
    }

    bool MutableStorageImpl::apply(
        rxcpp::observable<std::shared_ptr<shared_model::interface::Block>>
            blocks,
        MutableStoragePredicate predicate) {
      return withSavepoint([&] {
        return blocks
            .all([&](auto block) { return this->apply(block, predicate); })
            .as_blocking()
            .first();
      });
    }

    boost::optional<std::shared_ptr<const iroha::LedgerState>>
    MutableStorageImpl::getLedgerState() const {
      return ledger_state_;
    }

    MutableStorageImpl::~MutableStorageImpl() {
      try {
        db_tx_.rollback();
      } catch (std::exception &e) {
        log_->warn("Apply has been failed. Reason: {}", e.what());
      }
    }

    void MutableStorageImpl::commit() {
      db_tx_.commit();
    }

  }  // namespace newstorage
}  // namespace iroha
