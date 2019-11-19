/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_QUERY_PAYLOAD_META_HPP
#define IROHA_SHARED_MODEL_QUERY_PAYLOAD_META_HPP

#include "interfaces/base/model_primitive.hpp"

#include "interfaces/common_objects/types.hpp"
#include "queries.pb.h"

namespace shared_model {

  /**
   * Class QueryPayloadMeta provides metadata of query payload
   * General note: this class is container for queries but not a base class.
   */
  class QueryPayloadMeta : public ModelPrimitive<QueryPayloadMeta> {
   public:
    explicit QueryPayloadMeta(iroha::protocol::QueryPayloadMeta &meta);

    /**
     * @return id of query creator
     */
    const types::AccountIdType &creatorAccountId() const;

    /**
     * Query counter - incremental variable reflect for number of sent to
     * system queries plus 1. Required for preventing replay attacks.
     * @return attached query counter
     */
    types::CounterType queryCounter() const;

    /**
     * @return time of creation
     */
    types::TimestampType createdTime() const;

    bool operator==(const ModelType &rhs) const override;

   private:
    const iroha::protocol::QueryPayloadMeta &meta_;
  };
}  // namespace shared_model
#endif  // IROHA_SHARED_MODEL_QUERY_PAYLOAD_META_HPP
