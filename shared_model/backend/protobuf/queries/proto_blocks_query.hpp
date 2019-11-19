/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_PROTO_BLOCKS_QUERY_HPP
#define IROHA_SHARED_MODEL_PROTO_BLOCKS_QUERY_HPP

#include "interfaces/queries/blocks_query.hpp"

#include "backend/protobuf/common_objects/signature.hpp"
#include "queries.pb.h"

namespace shared_model {
  class BlocksQuery final : public BlocksQuery {
   public:
    using TransportType = iroha::protocol::BlocksQuery;

    explicit BlocksQuery(const TransportType &query);
    explicit BlocksQuery(TransportType &&query);

    const types::AccountIdType &creatorAccountId() const override;

    types::CounterType queryCounter() const override;

    const types::BlobType &blob() const override;

    const types::BlobType &payload() const override;

    // ------------------------| Signable override  |-------------------------
    types::SignatureRangeType signatures() const override;

    bool addSignature(const crypto::Signed &signed_blob,
                      const crypto::PublicKey &public_key) override;

    const types::HashType &hash() const override;

    types::TimestampType createdTime() const override;

    const TransportType &getTransport() const;

   private:
    // ------------------------------| fields |-------------------------------
    TransportType proto_;

    const types::BlobType blob_;

    const types::BlobType payload_;

    SignatureSetType<Signature> signatures_;

    types::HashType hash_;
  };
}  // namespace shared_model

#endif  // IROHA_SHARED_MODEL_PROTO_BLOCKS_QUERY_HPP
