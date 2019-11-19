/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_PROTO_BLOCK_RESPONSE_HPP
#define IROHA_SHARED_MODEL_PROTO_BLOCK_RESPONSE_HPP

#include "interfaces/query_responses/block_response.hpp"

#include "backend/protobuf/block.hpp"
#include "qry_responses.pb.h"

namespace shared_model {
  class BlockResponse final : public BlockResponse {
   public:
    explicit BlockResponse(iroha::protocol::BlockResponse &block_response);

    explicit BlockResponse(
        iroha::protocol::BlockQueryResponse &block_query_response);

    explicit BlockResponse(iroha::protocol::QueryResponse &query_response);

    const Block &block() const override;

   private:
    Block block_;
  };
}  // namespace shared_model

#endif  // IROHA_SHARED_MODEL_PROTO_BLOCK_RESPONSE_HPP
