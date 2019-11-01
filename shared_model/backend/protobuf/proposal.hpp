/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_PROTO_PROPOSAL_HPP
#define IROHA_SHARED_MODEL_PROTO_PROPOSAL_HPP

#include "interfaces/iroha_internal/proposal.hpp"

#include "interfaces/common_objects/types.hpp"
#include "proposal.pb.h"

namespace shared_model {
  class Proposal final : public Proposal {
   public:
    using TransportType = iroha::protocol::Proposal;

    Proposal(Proposal &&o) noexcept;
    Proposal &operator=(Proposal &&o) noexcept = default;

    explicit Proposal(const TransportType &ref);
    explicit Proposal(TransportType &&ref);

    types::TransactionsCollectionType transactions() const override;

    types::TimestampType createdTime() const override;

    types::HeightType height() const override;

    const types::BlobType &blob() const override;

    const TransportType &getTransport() const;

    const types::HashType &hash() const override;

    ~Proposal() override;

   private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
  };
}  // namespace shared_model

#endif  // IROHA_PROPOSAL_HPP
