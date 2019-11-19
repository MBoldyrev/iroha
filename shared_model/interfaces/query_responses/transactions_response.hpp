/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_TRANSACTIONS_RESPONSE_HPP
#define IROHA_SHARED_MODEL_TRANSACTIONS_RESPONSE_HPP

#include "interfaces/base/model_primitive.hpp"

#include "interfaces/common_objects/range_types.hpp"
#include "interfaces/common_objects/types.hpp"

namespace shared_model {
  /**
   * Container of asset, for fetching data.
   */
  class TransactionsResponse : public ModelPrimitive<TransactionsResponse> {
   public:
    /**
     * @return Attached transactions
     */
    types::TransactionsCollectionType transactions() const;

    std::string toString() const override;

    bool operator==(const ModelType &rhs) const override;
  };
}  // namespace shared_model
#endif  // IROHA_SHARED_MODEL_TRANSACTIONS_RESPONSE_HPP
