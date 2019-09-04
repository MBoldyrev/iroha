/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_NEWSTORAGE_BLOCK_STORAGE_FACTORY_HPP
#define IROHA_NEWSTORAGE_BLOCK_STORAGE_FACTORY_HPP

#include "ametsuchi/block_storage_factory.hpp"
#include "backend/protobuf/proto_block_factory.hpp"
#include "logger/logger_fwd.hpp"

namespace iroha {
  namespace newstorage {
   class BlockStorageFactory : public ametsuchi::BlockStorageFactory {
     public:
      BlockStorageFactory(
          std::shared_ptr<shared_model::proto::ProtoBlockFactory> block_factory,
          std::string location,
          logger::LoggerPtr log);

      std::unique_ptr<ametsuchi::BlockStorage> create() override;

     private:
      std::shared_ptr<shared_model::proto::ProtoBlockFactory> block_factory_;
      std::string location_;
      logger::LoggerPtr log_;
    };
  }  // namespace ametsuchi
}  // namespace iroha

#endif  // IROHA_NEWSTORAGE_BLOCK_STORAGE_FACTORY_HPP
