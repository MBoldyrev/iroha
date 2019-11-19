/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "interfaces/iroha_internal/block_json_converter.hpp"

#include <google/protobuf/util/json_util.h>
#include <string>

#include "interfaces/block.hpp"

using namespace shared_model;
using namespace shared_model::proto;

iroha::expected::Result<types::JsonType, std::string>
ProtoBlockJsonConverter::serialize(const Block &block) const noexcept {
  const auto &proto_block_v1 = static_cast<const Block &>(block).getTransport();
  iroha::protocol::Block proto_block;
  *proto_block.mutable_block_v1() = proto_block_v1;
  std::string result;
  auto status =
      google::protobuf::util::MessageToJsonString(proto_block, &result);

  if (not status.ok()) {
    return iroha::expected::makeError(status.error_message());
  }
  return iroha::expected::makeValue(result);
}

iroha::expected::Result<std::unique_ptr<Block>, std::string>
ProtoBlockJsonConverter::deserialize(const types::JsonType &json) const
    noexcept {
  iroha::protocol::Block block;
  auto status = google::protobuf::util::JsonStringToMessage(json, &block);
  if (not status.ok()) {
    return iroha::expected::makeError(status.error_message());
  }
  std::unique_ptr<Block> result =
      std::make_unique<Block>(std::move(block.block_v1()));
  return iroha::expected::makeValue(std::move(result));
}
