/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "backend/protobuf/query_responses/proto_block_response.hpp"

BlockResponse::BlockResponse(iroha::protocol::BlockResponse &block_response)
    : block_{*block_response.mutable_block()->mutable_block_v1()} {}

BlockResponse::BlockResponse(
    iroha::protocol::BlockQueryResponse &block_query_response)
    : BlockResponse(*block_query_response.mutable_block_response()) {}

BlockResponse::BlockResponse(iroha::protocol::QueryResponse &query_response)
    : BlockResponse(*query_response.mutable_block_response()) {}

const Block &BlockResponse::block() const {
  return block_;
}
