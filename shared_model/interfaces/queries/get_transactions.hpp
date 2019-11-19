/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_GET_TRANSACTIONS_HPP
#define IROHA_SHARED_MODEL_GET_TRANSACTIONS_HPP

#include "interfaces/base/model_primitive.hpp"

#include "cryptography/hash.hpp"
#include "interfaces/common_objects/types.hpp"
#include "queries.pb.h"

namespace shared_model {
  class GetTransactions : public ModelPrimitive<GetTransactions> {
   public:
    explicit GetTransactions(iroha::protocol::Query &query);

    /// type of hashes collection
    using TransactionHashesType = std::vector<types::HashType>;

    /**
     * @return Hashes of transactions to fetch
     */
    const TransactionHashesType &transactionHashes() const;

    std::string toString() const override;

    bool operator==(const ModelType &rhs) const override;

   private:
    // ------------------------------| fields |-------------------------------

    const iroha::protocol::GetTransactions &get_transactions_;

    const TransactionHashesType transaction_hashes_;
  };
}  // namespace shared_model

#endif  // IROHA_SHARED_MODEL_GET_TRANSACTIONS_HPP
