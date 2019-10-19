/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_NEWSTORAGE_FACTORY_HPP
#define IROHA_NEWSTORAGE_FACTORY_HPP

#include "ametsuchi/storage.hpp"
#include "logger/logger_manager_fwd.hpp"

namespace shared_model {
  namespace interface {
    class QueryResponseFactory;
    class PermissionToString;
    class Block;
    class Peer;
  }  // namespace interface
}  // namespace shared_model

namespace iroha {
  class PendingTransactionStorage;

  namespace newstorage {

    class StorageFactory : public ametsuchi::StorageFactory {
     public:
      // file/dir suffixes
        static const char *kWsvDB;
        static const char *kIndexDB;
        static const char *kBlocks;
        static const char *kProps;

      struct Options {
        std::string path;
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
      };

      explicit StorageFactory(Options &&options)
          : options_(std::move(options)) {}

      ~StorageFactory() override = default;

      std::shared_ptr<ametsuchi::Storage> create() override;

     private:
      Options options_;
    };

  }  // namespace newstorage
}  // namespace iroha

#endif  // IROHA_NEWSTORAGE_FACTORY_HPP
