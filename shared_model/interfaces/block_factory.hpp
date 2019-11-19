/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_PROTO_BLOCK_FACTORY_HPP
#define IROHA_PROTO_BLOCK_FACTORY_HPP

#include "block.pb.h"
#include "common/result.hpp"
#include "interfaces/iroha_internal/unsafe_block_factory.hpp"
#include "interfaces/transaction.hpp"
#include "validators/abstract_validator.hpp"

namespace shared_model {
  /**
   * ProtoBlockFactory is used to create Block objects
   */
  class ProtoBlockFactory : public UnsafeBlockFactory {
   public:
    ProtoBlockFactory(
        std::unique_ptr<shared_model::validation::AbstractValidator<
            shared_model::Block>> interface_validator,
        std::unique_ptr<
            shared_model::validation::AbstractValidator<iroha::protocol::Block>>
            proto_validator);

    std::unique_ptr<Block> unsafeCreateBlock(
        types::HeightType height,
        const types::HashType &prev_hash,
        types::TimestampType created_time,
        const types::TransactionsCollectionType &txs,
        const types::HashCollectionType &rejected_hashes) override;

    /**
     * Create block variant
     *
     * @param block - proto block from which block variant is created
     * @return Pointer to block.
     *         Error if block is invalid
     */
    iroha::expected::Result<std::unique_ptr<Block>, std::string> createBlock(
        iroha::protocol::Block block);

   private:
    std::unique_ptr<
        shared_model::validation::AbstractValidator<shared_model::Block>>
        interface_validator_;
    std::unique_ptr<
        shared_model::validation::AbstractValidator<iroha::protocol::Block>>
        proto_validator_;
  };
}  // namespace shared_model

#endif  // IROHA_PROTO_BLOCK_FACTORY_HPP
