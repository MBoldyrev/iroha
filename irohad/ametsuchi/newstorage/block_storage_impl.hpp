/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_BLOCK_STORAGE_IMPL_HPP
#define IROHA_BLOCK_STORAGE_IMPL_HPP

#include "ametsuchi/block_storage.hpp"
#include "ametsuchi/newstorage/keyvalue_db_backend.hpp"
#include "backend/protobuf/block.hpp"
#include "backend/protobuf/proto_block_factory.hpp"
#include "interfaces/iroha_internal/abstract_transport_factory.hpp"
#include "logger/logger_fwd.hpp"

namespace iroha {
  namespace newstorage {
   class BlockStorageImpl : public ametsuchi::BlockStorage {
     public:
      using BlockTransportFactory = shared_model::proto::ProtoBlockFactory;

      BlockStorageImpl(std::shared_ptr<BlockTransportFactory> block_factory,
                       const std::string& location,
                       logger::LoggerPtr log);

      bool insert(
          std::shared_ptr<const shared_model::interface::Block> block) override;

      boost::optional<std::shared_ptr<const shared_model::interface::Block>>
      fetch(shared_model::interface::types::HeightType height) const override;

      // TODO returns height
      size_t size() const override;

      void clear() override;

      void forEach(FunctionType function) const override;

     private:
     boost::optional<std::shared_ptr<const shared_model::interface::Block>>
     deserializeBlock(uint64_t height, const void* blob, size_t size) const;

      std::shared_ptr<BlockTransportFactory> block_factory_;
      logger::LoggerPtr log_;
      KeyValueDbBackend db_;
      uint64_t height_;
    };

  }  // namespace newstorage
}  // namespace iroha

#endif  // IROHA_BLOCK_STORAGE_IMPL_HPP
