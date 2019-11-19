/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_PROTO_QUERY_PAYLOAD_META_HPP
#define IROHA_SHARED_MODEL_PROTO_QUERY_PAYLOAD_META_HPP

#include "interfaces/queries/query_payload_meta.hpp"

#include "queries.pb.h"

namespace shared_model {
  class QueryPayloadMeta final : public QueryPayloadMeta {
   public:
    explicit QueryPayloadMeta(iroha::protocol::QueryPayloadMeta &meta);

    const types::AccountIdType &creatorAccountId() const override;

    types::CounterType queryCounter() const override;

    types::TimestampType createdTime() const override;

   private:
    const iroha::protocol::QueryPayloadMeta &meta_;
  };
}  // namespace shared_model

#endif  // IROHA_SHARED_MODEL_PROTO_QUERY_HPP
