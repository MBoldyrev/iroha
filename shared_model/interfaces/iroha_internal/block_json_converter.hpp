/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_BLOCK_JSON_CONVERTER_HPP
#define IROHA_BLOCK_JSON_CONVERTER_HPP

#include "interfaces/iroha_internal/block_json_deserializer.hpp"
#include "interfaces/iroha_internal/block_json_serializer.hpp"

namespace shared_model {
  class Block;

  /**
   * Block json converter is a class which can convert blocks to/from json
   */
  class BlockJsonConverter : public BlockJsonSerializer,
                             public BlockJsonDeserializer {
   public:
    ~BlockJsonConverter() override = default;

    iroha::expected::Result<types::JsonType, std::string> serialize(
        const Block &block) const noexcept override;

    iroha::expected::Result<std::unique_ptr<Block>, std::string> deserialize(
        const types::JsonType &json) const noexcept override;
  };
}  // namespace shared_model

#endif  // IROHA_BLOCK_JSON_CONVERTER_HPP
