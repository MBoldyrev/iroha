/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_BLOCKS_QUERY_HPP
#define IROHA_SHARED_MODEL_BLOCKS_QUERY_HPP

#include "interfaces/base/signable.hpp"

#include "interfaces/common_objects/signature.hpp"
#include "interfaces/common_objects/types.hpp"
#include "queries.pb.h"

namespace shared_model {

  /**
   * Class BlocksQuery provides container with one of concrete query available
   * in system. General note: this class is container for queries but not a
   * base class.
   */
  class BlocksQuery : public Signable<BlocksQuery> {
   public:
    using TransportType = iroha::protocol::BlocksQuery;

    explicit BlocksQuery(const TransportType &query);
    explicit BlocksQuery(TransportType &&query);

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

    // ------------------------| Primitive override |-------------------------

    std::string toString() const override;

    bool operator==(const ModelType &rhs) const override;

   private:
    // ------------------------------| fields |-------------------------------
    TransportType proto_;

    const types::BlobType blob_;

    const types::BlobType payload_;

    SignatureSetType<Signature> signatures_;

    types::HashType hash_;
  };
}  // namespace shared_model
#endif  // IROHA_SHARED_MODEL_BLOCKS_QUERY_HPP
