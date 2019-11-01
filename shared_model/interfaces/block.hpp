/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_BLOCK_HPP
#define IROHA_SHARED_MODEL_BLOCK_HPP

#include "common/cloneable.hpp"
#include "interfaces/base/signable.hpp"
#include "interfaces/common_objects/types.hpp"

namespace shared_model {

  class Block : public Signable<Block>, public Cloneable<Block> {
   public:
    /**
     * @return block number in the ledger
     */
    types::HeightType height() const;

    /**
     * @return hash of a previous block
     */
    const types::HashType &prevHash() const;
    /**
     * @return amount of transactions in block
     */
    types::TransactionsNumberType txsNumber() const;

    /**
     * @return collection of transactions
     */
    types::TransactionsCollectionType transactions() const;

    /**
     * @return collection of rejected transactions' hashes
     */
    types::HashCollectionType rejected_transactions_hashes() const;

    std::string toString() const override;
  };

}  // namespace shared_model

#endif  // IROHA_SHARED_MODEL_BLOCK_HPP
