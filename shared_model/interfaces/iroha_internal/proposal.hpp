/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_PROPOSAL_HPP
#define IROHA_SHARED_MODEL_PROPOSAL_HPP

#include "cryptography/default_hash_provider.hpp"
#include "interfaces/base/model_primitive.hpp"
#include "interfaces/common_objects/types.hpp"
#include "interfaces/transaction.hpp"

namespace shared_model {

  class Proposal : public ModelPrimitive<Proposal> {
   public:
    using TransportType = iroha::protocol::Proposal;

    Proposal(Proposal &&o) noexcept;
    Proposal &operator=(Proposal &&o) noexcept = default;

    explicit Proposal(const TransportType &ref);
    explicit Proposal(TransportType &&ref);

    /**
     * @return transactions
     */
    types::TransactionsCollectionType transactions() const;

    /**
     * @return the height
     */
    types::HeightType height() const;

    /**
     * @return created time
     */
    types::TimestampType createdTime() const;

    bool operator==(const Proposal &rhs) const override {
      return transactions() == rhs.transactions() and height() == rhs.height()
          and createdTime() == rhs.createdTime();
    }

    const types::BlobType &blob() const;

    const types::HashType &hash() const;

    std::string toString() const override {
      return detail::PrettyStringBuilder()
          .init("Proposal")
          .append("height", std::to_string(height()))
          .append("transactions")
          .appendAll(transactions(),
                     [](auto &transaction) { return transaction.toString(); })
          .finalize();
    }

   private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
  };

}  // namespace shared_model

#endif  // IROHA_SHARED_MODEL_PROPOSAL_HPP
