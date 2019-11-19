/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_PROTO_TRANSACTION_HPP
#define IROHA_SHARED_MODEL_PROTO_TRANSACTION_HPP

#include "interfaces/transaction.hpp"
#include "transaction.pb.h"

namespace shared_model {
  class Transaction final : public Transaction {
   public:
    using TransportType = iroha::protocol::Transaction;

    explicit Transaction(const TransportType &transaction);

    explicit Transaction(TransportType &&transaction);

    explicit Transaction(TransportType &transaction);

    Transaction(const Transaction &transaction);

    Transaction(Transaction &&o) noexcept;

    ~Transaction() override;

    const types::AccountIdType &creatorAccountId() const override;

    Transaction::CommandsType commands() const override;

    const types::BlobType &blob() const override;

    const types::BlobType &payload() const override;

    const types::BlobType &reducedPayload() const override;

    types::SignatureRangeType signatures() const override;

    const types::HashType &reducedHash() const override;

    bool addSignature(const crypto::Signed &signed_blob,
                      const crypto::PublicKey &public_key) override;

    const types::HashType &hash() const override;

    const TransportType &getTransport() const;

    types::TimestampType createdTime() const override;

    types::QuorumType quorum() const override;

    boost::optional<std::shared_ptr<BatchMeta>> batchMeta() const override;

   protected:
    Transaction::ModelType *clone() const override;

   private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
  };
}  // namespace shared_model

#endif  // IROHA_SHARED_MODEL_PROTO_TRANSACTION_HPP
