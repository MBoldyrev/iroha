/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ametsuchi/newstorage/block_storage_impl.hpp"

#include "common/hexutils.hpp"
#include "logger/logger.hpp"

namespace iroha::newstorage {

  BlockStorageImpl::BlockStorageImpl(
      std::shared_ptr<BlockTransportFactory> block_factory,
      const std::string& location,
      logger::LoggerPtr log)
      : block_factory_(std::move(block_factory)),
        log_(std::move(log)),
        db_({location}, std::move(log), false)
      {
        db_.create();
        db_.get_last_num_key(height_);
      }

  bool BlockStorageImpl::insert(
      std::shared_ptr<const shared_model::interface::Block> block) {

    if (height_ != 0) {
      if (block->height() != height_ + 1) {
        log_->warn(
            "Only blocks with sequential heights could be inserted. Last block "
            "height: {}, inserting: {}",
            height_,
            block->height());
        return false;
      }
    }

    auto h = block->height();
    auto b = block->blob().blob();

    try {
      db_.put(h, slice(b));
      ++height_;
      return true;
    } catch (const std::exception &e) {
      log_->warn("Failed to insert block {}, reason {}", h, e.what());
      return false;
    }
  }

  boost::optional<std::shared_ptr<const shared_model::interface::Block>>
  BlockStorageImpl::deserializeBlock(uint64_t height, const void* blob, size_t size) const {
      iroha::protocol::Block_v1 b1;
      b1.ParseFromArray(blob, size); // TODO return values!
      iroha::protocol::Block block;
      *block.mutable_block_v1() = b1;

      return block_factory_->createBlock(std::move(block))
          .match(
              [&](auto &&v) {
                return boost::make_optional(
                    std::shared_ptr<const shared_model::interface::Block>(
                        std::move(v.value)));
              },
              [&](const auto &e)
                  -> boost::optional<std::shared_ptr<
                  const shared_model::interface::Block>> {
                log_->error("Could not build block at height {}: {}",
                            height,
                            e.error);
                return boost::none;
              });
    }

  boost::optional<std::shared_ptr<const shared_model::interface::Block>>
  BlockStorageImpl::fetch(
      shared_model::interface::types::HeightType height) const {

    std::string blob = db_.get(height);
    if (blob.empty()) {
      log_->error("Could not find block at height {}", height);
      return boost::optional<
          std::shared_ptr<const shared_model::interface::Block>>(
          boost::none);
    }

    return deserializeBlock(height, blob.data(), blob.size());
  }

  size_t BlockStorageImpl::size() const {
    return height_;
  }

  void BlockStorageImpl::clear() {
    // TODO drop everything
  }

  void BlockStorageImpl::forEach(
      iroha::ametsuchi::BlockStorage::FunctionType function) const {
      db_.iterate_from(
          0,
          [&function, this](uint64_t h, const Slice& blob) -> bool {
            auto block = deserializeBlock(h, blob.data(), blob.size());
            if (!block) return false;
            function(*block);
            return true;
          }
      );
  }

} //namespace
