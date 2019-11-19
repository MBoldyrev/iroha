/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_PROTO_BLOCK_JSON_CONVERTER_HPP
#define IROHA_PROTO_BLOCK_JSON_CONVERTER_HPP

#include "interfaces/common_objects/types.hpp"
#include "interfaces/iroha_internal/block_json_converter.hpp"

namespace shared_model {
  namespace interface {
    class Block;
  }

  class ProtoBlockJsonConverter : public BlockJsonConverter {
   public:
    iroha::expected::Result<types::JsonType, std::string> serialize(
        const Block &block) const noexcept override;

    iroha::expected::Result<std::unique_ptr<Block>, std::string> deserialize(
        const types::JsonType &json) const noexcept override;
  };
}  // namespace shared_model

#endif  // IROHA_PROTO_BLOCK_JSON_CONVERTER_HPP
