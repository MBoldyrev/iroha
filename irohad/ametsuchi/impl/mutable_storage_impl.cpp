/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ametsuchi/impl/mutable_storage_impl.hpp"

#include <boost/variant/apply_visitor.hpp>
#include "ametsuchi/impl/peer_query_wsv.hpp"
#include "ametsuchi/impl/postgres_block_index.hpp"
#include "ametsuchi/impl/postgres_command_executor.hpp"
#include "ametsuchi/impl/postgres_wsv_command.hpp"
#include "ametsuchi/impl/postgres_wsv_query.hpp"
#include "ametsuchi/ledger_state.hpp"
#include "interfaces/commands/command.hpp"
#include "interfaces/common_objects/common_objects_factory.hpp"
#include "interfaces/iroha_internal/block.hpp"
#include "logger/logger.hpp"
#include "logger/logger_manager.hpp"

namespace iroha {
  namespace ametsuchi {
    MutableStorageImpl::MutableStorageImpl(
        boost::optional<std::shared_ptr<const iroha::LedgerState>> ledger_state,
        std::shared_ptr<PostgresCommandExecutor> cmd_executor,
        std::unique_ptr<SociSession> sql,
        std::shared_ptr<shared_model::interface::CommonObjectsFactory> factory,
        std::unique_ptr<BlockStorage> block_storage,
        logger::LoggerManagerTreePtr log_manager)
        : ledger_state_(std::move(ledger_state)),
          sql_(std::move(sql)),
          peer_query_(
              std::make_unique<PeerQueryWsv>(std::make_shared<PostgresWsvQuery>(
                  *sql_,
                  std::move(factory),
                  log_manager->getChild("WsvQuery")->getLogger()))),
          block_index_(std::make_unique<PostgresBlockIndex>(
              *sql_, log_manager->getChild("PostgresBlockIndex")->getLogger())),
          command_executor_(std::move(cmd_executor)),
          block_storage_(std::move(block_storage)),
          committed(false),
          log_(log_manager->getLogger()) {
      *sql_ << "BEGIN";
    }

    bool MutableStorageImpl::apply(
        std::shared_ptr<const shared_model::interface::Block> block,
        MutableStoragePredicate predicate) {
      auto execute_transaction = [this](auto &transaction) {
        command_executor_->setCreatorAccountId(transaction.creatorAccountId());
        command_executor_->doValidation(false);

        auto execute_command = [this](const auto &command) {
          auto command_applied =
              boost::apply_visitor(*command_executor_, command.get());

          return command_applied.match([](const auto &) { return true; },
                                       [&](const auto &e) {
                                         log_->error(e.error.toString());
                                         return false;
                                       });
        };

        return std::all_of(transaction.commands().begin(),
                           transaction.commands().end(),
                           execute_command);
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

        auto opt_ledger_peers = peer_query_->getLedgerPeers();
        if (not opt_ledger_peers) {
          log_->error("Failed to get ledger peers!");
          return false;
        }
        assert(opt_ledger_peers);

        // atomic?
        ledger_state_ = std::make_shared<const LedgerState>(
            std::move(*opt_ledger_peers), block->height(), block->hash());
      }

      return block_applied;
    }

    template <typename Function>
    bool MutableStorageImpl::withSavepoint(Function &&function) {
      try {
        *sql_ << "SAVEPOINT savepoint_";

        auto function_executed = std::forward<Function>(function)();

        if (function_executed) {
          *sql_ << "RELEASE SAVEPOINT savepoint_";
        } else {
          *sql_ << "ROLLBACK TO SAVEPOINT savepoint_";
        }
        return function_executed;
      } catch (std::exception &e) {
        log_->warn("Apply has failed. Reason: {}", e.what());
        return false;
      }
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
      if (not committed) {
        try {
          *sql_ << "ROLLBACK";
        } catch (std::exception &e) {
          log_->warn("Apply has been failed. Reason: {}", e.what());
        }
      }
    }
  }  // namespace ametsuchi
}  // namespace iroha
