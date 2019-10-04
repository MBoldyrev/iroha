/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_NEWSTORAGE_IMPL_HPP
#define IROHA_NEWSTORAGE_IMPL_HPP

#include "ametsuchi/storage.hpp"
#include "ametsuchi/newstorage/mutable_wsv.hpp"

#include <atomic>
#include <shared_mutex>

#include <boost/optional.hpp>
#include "ametsuchi/block_storage_factory.hpp"
#include "ametsuchi/key_value_storage.hpp"
#include "ametsuchi/ledger_state.hpp"
#include "common/result.hpp"
#include "interfaces/permission_to_string.hpp"
#include "logger/logger_fwd.hpp"
#include "logger/logger_manager_fwd.hpp"

namespace shared_model {
  namespace interface {
    class QueryResponseFactory;
  }  // namespace interface
}  // namespace shared_model

namespace iroha {

  class PendingTransactionStorage;

  namespace newstorage {

   class StorageImpl : public ametsuchi::Storage {
     public:
      static expected::Result<std::shared_ptr<StorageImpl>, std::string> create(
          std::shared_ptr<shared_model::interface::PermissionToString>
              perm_converter,
          std::shared_ptr<PendingTransactionStorage> pending_txs_storage,
          std::shared_ptr<shared_model::interface::QueryResponseFactory>
              query_response_factory,
          logger::LoggerManagerTreePtr log_manager);

      expected::Result<std::unique_ptr<ametsuchi::CommandExecutor>, std::string>
      createCommandExecutor() override;

      std::unique_ptr<ametsuchi::TemporaryWsv> createTemporaryWsv(
          std::shared_ptr<ametsuchi::CommandExecutor> command_executor) override;

      std::unique_ptr<ametsuchi::MutableStorage> createMutableStorage(
          std::shared_ptr<ametsuchi::CommandExecutor> command_executor) override;

      boost::optional<std::shared_ptr<ametsuchi::PeerQuery>> createPeerQuery()
          const override;

      boost::optional<std::shared_ptr<ametsuchi::BlockQuery>> createBlockQuery()
          const override;

      boost::optional<std::shared_ptr<ametsuchi::QueryExecutor>> createQueryExecutor(
          std::shared_ptr<PendingTransactionStorage> pending_txs_storage,
          std::shared_ptr<shared_model::interface::QueryResponseFactory>
              response_factory) const override;

      bool insertBlock(
          std::shared_ptr<const shared_model::interface::Block> block) override;

      expected::Result<void, std::string> insertPeer(
          const shared_model::interface::Peer &peer) override;

      std::unique_ptr<ametsuchi::MutableStorage> createMutableStorage(
          std::shared_ptr<ametsuchi::CommandExecutor> command_executor,
          ametsuchi::BlockStorageFactory &storage_factory) override;

      void reset() override;

      expected::Result<void, std::string> resetWsv() override;

      void resetPeers() override;

      void dropStorage() override;

      void freeConnections() override;

     ametsuchi::CommitResult commit(
          std::unique_ptr<ametsuchi::MutableStorage> mutable_storage) override;

      bool preparedCommitEnabled() const override;

     ametsuchi::CommitResult commitPrepared(
          std::shared_ptr<const shared_model::interface::Block> block) override;

      std::shared_ptr<ametsuchi::WsvQuery> getWsvQuery() const override;

      std::shared_ptr<ametsuchi::BlockQuery> getBlockQuery() const override;

      rxcpp::observable<std::shared_ptr<const shared_model::interface::Block>>
      on_commit() override;

      void prepareBlock(std::unique_ptr<ametsuchi::TemporaryWsv> wsv) override;

      ~StorageImpl() override;

     protected:
      StorageImpl(
          const std::string& location,
          std::shared_ptr<shared_model::interface::PermissionToString>
              perm_converter,
          std::shared_ptr<PendingTransactionStorage> pending_txs_storage,
          std::shared_ptr<shared_model::interface::QueryResponseFactory>
              query_response_factory,
          logger::LoggerManagerTreePtr log_manager);


     private:
      using StoreBlockResult = iroha::expected::Result<void, std::string>;

      /**
       * add block to block storage
       */
      StoreBlockResult storeBlock(
          std::shared_ptr<const shared_model::interface::Block> block);


      std::unique_ptr<ametsuchi::BlockStorage> block_store_;

      rxcpp::composite_subscription notifier_lifetime_;
      rxcpp::subjects::subject<
          std::shared_ptr<const shared_model::interface::Block>>
          notifier_;

      std::shared_ptr<shared_model::interface::PermissionToString>
          perm_converter_;

      std::shared_ptr<PendingTransactionStorage> pending_txs_storage_;

      std::shared_ptr<shared_model::interface::QueryResponseFactory>
          query_response_factory_;

      std::unique_ptr<ametsuchi::BlockStorageFactory> temporary_block_storage_factory_;

      logger::LoggerManagerTreePtr log_manager_;
      logger::LoggerPtr log_;

      mutable std::shared_timed_mutex drop_mutex_;

      bool prepared_blocks_enabled_;

      std::atomic<bool> block_is_prepared_;

      boost::optional<std::shared_ptr<const iroha::LedgerState>> ledger_state_;

      MutableWsv mutable_wsv_;
    };
  }  // namespace newstorage
}  // namespace iroha

#endif  // IROHA_NEWSTORAGE_IMPL_HPP
