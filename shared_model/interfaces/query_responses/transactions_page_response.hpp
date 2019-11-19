/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_TRANSACTIONS_PAGE_RESPONSE_HPP
#define IROHA_SHARED_MODEL_TRANSACTIONS_PAGE_RESPONSE_HPP

#include "interfaces/base/model_primitive.hpp"

#include <boost/optional/optional_fwd.hpp>
#include "interfaces/common_objects/range_types.hpp"
#include "interfaces/common_objects/types.hpp"

namespace shared_model {
  /**
   * Response for paginated queries
   */
  class TransactionsPageResponse
      : public ModelPrimitive<TransactionsPageResponse> {
   public:
    /**
     * @return transactions from this page
     */
    types::TransactionsCollectionType transactions() const;

    /**
     * @return hash of the first transaction from the next page
     */
    boost::optional<types::HashType> nextTxHash() const;

    /**
     * @return total number of transactions for the query
     */
    types::TransactionsNumberType allTransactionsSize() const;

    std::string toString() const override;

    bool operator==(const ModelType &rhs) const override;
  };
}  // namespace shared_model
#endif  // IROHA_SHARED_MODEL_TRANSACTIONS_PAGE_RESPONSE_HPP
