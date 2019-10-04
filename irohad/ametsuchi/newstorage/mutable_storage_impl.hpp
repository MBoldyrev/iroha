/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_MUTABLE_NEWSTORAGE_IMPL_HPP
#define IROHA_MUTABLE_NEWSTORAGE_IMPL_HPP

#include "ametsuchi/block_storage.hpp"
#include "ametsuchi/mutable_storage.hpp"
#include "ametsuchi/newstorage/mutable_wsv.hpp"
#include "interfaces/common_objects/types.hpp"
#include "logger/logger_fwd.hpp"
#include "logger/logger_manager_fwd.hpp"

namespace iroha {
  namespace ametsuchi {
    class TransactionExecutor;
    class CommandExecutor;
  }  // namespace ametsuchi

  namespace newstorage {
    class MutableWsv;
    class BlockIndexDB;
    class BlockIndex;
    class WsvQueryImpl;

    class MutableStorageImpl : public ametsuchi::MutableStorage {
     public:
      MutableStorageImpl(
          boost::optional<std::shared_ptr<const iroha::LedgerState>>
              ledger_state,
          std::shared_ptr<ametsuchi::CommandExecutor> command_executor,
          MutableWsv &db,
          std::shared_ptr<BlockIndexDB> index_db,
          std::unique_ptr<ametsuchi::BlockStorage> block_storage,
          logger::LoggerManagerTreePtr log_manager);

      bool apply(
          std::shared_ptr<const shared_model::interface::Block> block) override;

      bool apply(rxcpp::observable<
                     std::shared_ptr<shared_model::interface::Block>> blocks,
                 MutableStoragePredicate predicate) override;

      boost::optional<std::shared_ptr<const iroha::LedgerState>>
      getLedgerState() const;

      ~MutableStorageImpl() override;

      void commit();

     private:
      /**
       * Performs a function inside savepoint, does a rollback if function
       * returned false, and removes the savepoint otherwise. Returns function
       * result
       */
      template <typename Function>
      bool withSavepoint(Function &&function);

      /**
       * Verifies whether the block is applicable using predicate, and applies
       * the block
       */
      bool apply(std::shared_ptr<const shared_model::interface::Block> block,
                 MutableStoragePredicate predicate);

      boost::optional<std::shared_ptr<const iroha::LedgerState>> ledger_state_;
      MutableWsv &db_;
      logger::LoggerPtr log_;
      std::unique_ptr<ametsuchi::BlockStorage> block_storage_;
      std::unique_ptr<WsvQueryImpl> peer_query_;
      std::unique_ptr<BlockIndex> block_index_;
      std::unique_ptr<ametsuchi::TransactionExecutor> transaction_executor_;
      DbTransaction db_tx_;
    };
  }  // namespace newstorage
}  // namespace iroha

#endif  // IROHA_MUTABLE_NEWSTORAGE_IMPL_HPP
