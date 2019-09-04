/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ametsuchi/newstorage/block_storage_factory.hpp"
#include "ametsuchi/newstorage/block_storage_impl.hpp"
#include "logger/logger.hpp"

namespace iroha { namespace newstorage {

  BlockStorageFactory::BlockStorageFactory(
      std::shared_ptr<shared_model::proto::ProtoBlockFactory> block_factory,
      std::string location,
      logger::LoggerPtr log)
      : block_factory_(std::move(block_factory)),
        location_(std::move(location)),
        log_(std::move(log)) {}

  std::unique_ptr<ametsuchi::BlockStorage> BlockStorageFactory::create() {

    return std::make_unique<BlockStorageImpl>(
         block_factory_, location_, log_);
  }

}} //namespaces