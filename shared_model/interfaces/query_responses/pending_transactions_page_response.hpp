/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_PENDING_TRANSACTIONS_PAGE_RESPONSE_HPP
#define IROHA_SHARED_MODEL_PENDING_TRANSACTIONS_PAGE_RESPONSE_HPP

#include "interfaces/base/model_primitive.hpp"

#include <boost/optional/optional_fwd.hpp>
#include "cryptography/hash.hpp"
#include "interfaces/common_objects/range_types.hpp"
#include "interfaces/common_objects/types.hpp"

namespace shared_model {

  /**
   * Response for paginated queries
   */
  class PendingTransactionsPageResponse
      : public ModelPrimitive<PendingTransactionsPageResponse> {
   public:
    // TODO igor-egorov 2019-06-29 IR-570 Convert BatchInfo to a shared model
    // object
    struct BatchInfo {
      types::HashType first_tx_hash;
      types::TransactionsNumberType batch_size;

      bool operator==(const BatchInfo &rhs) const {
        return first_tx_hash == rhs.first_tx_hash
            and batch_size == rhs.batch_size;
      }
    };

    /**
     * @return transactions from this page
     */
    types::TransactionsCollectionType transactions() const;

    /**
     * @return next batch info to query the following page if exists
     */
    boost::optional<BatchInfo> nextBatchInfo() const;

    /**
     * @return total number of transactions for the query
     */
    types::TransactionsNumberType allTransactionsSize() const;

    std::string toString() const override;

    bool operator==(const ModelType &rhs) const override;
  };
}  // namespace shared_model

#endif  // IROHA_SHARED_MODEL_PENDING_TRANSACTIONS_PAGE_RESPONSE_HPP
