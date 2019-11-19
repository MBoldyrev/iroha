/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_PROTO_QUERY_HPP
#define IROHA_SHARED_MODEL_PROTO_QUERY_HPP

#include "interfaces/queries/query.hpp"

#include "queries.pb.h"

namespace shared_model {

  class Query final : public Query {
   public:
    using TransportType = iroha::protocol::Query;

    Query(const Query &o);
    Query(Query &&o) noexcept;

    explicit Query(const TransportType &ref);
    explicit Query(TransportType &&ref);

    ~Query() override;

    const Query::QueryVariantType &get() const override;

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
    struct Impl;
    std::unique_ptr<Impl> impl_;
  };

}  // namespace shared_model

#endif  // IROHA_SHARED_MODEL_PROTO_QUERY_HPP
