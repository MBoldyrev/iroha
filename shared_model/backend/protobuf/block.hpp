/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_PROTO_BLOCK_HPP
#define IROHA_SHARED_MODEL_PROTO_BLOCK_HPP

#include "interfaces/iroha_internal/block.hpp"

#include "block.pb.h"
#include "interfaces/common_objects/types.hpp"

namespace shared_model {
  class Block final : public Block {
   public:
    using TransportType = iroha::protocol::Block_v1;

    Block(Block &&o) noexcept;
    Block &operator=(Block &&o) noexcept = default;
    explicit Block(const TransportType &ref);
    explicit Block(TransportType &&ref);

    types::TransactionsCollectionType transactions() const override;

    types::HeightType height() const override;

    const types::HashType &prevHash() const override;

    const types::BlobType &blob() const override;

    types::SignatureRangeType signatures() const override;

    bool addSignature(const crypto::Signed &signed_blob,
                      const crypto::PublicKey &public_key) override;

    const types::HashType &hash() const override;

    types::TimestampType createdTime() const override;

    types::TransactionsNumberType txsNumber() const override;

    types::HashCollectionType rejected_transactions_hashes() const override;

    const types::BlobType &payload() const override;

    typename Block::ModelType *clone() const override;

    const iroha::protocol::Block_v1 &getTransport() const;

    ~Block() override;

   private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
  };
}  // namespace shared_model

#endif  // IROHA_SHARED_MODEL_PROTO_BLOCK_HPP
