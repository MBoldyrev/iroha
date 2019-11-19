/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_ACCOUNT_DETAIL_RESPONSE_HPP
#define IROHA_SHARED_MODEL_ACCOUNT_DETAIL_RESPONSE_HPP

#include "interfaces/base/model_primitive.hpp"

#include <boost/optional.hpp>
#include "interfaces/common_objects/types.hpp"
#include "interfaces/queries/account_detail_record_id.hpp"

namespace shared_model {
  /**
   * Provide response with account asset
   */
  class AccountDetailResponse : public ModelPrimitive<AccountDetailResponse> {
   public:
    /**
     * @return Account has Asset model
     */
    const types::DetailType &detail() const;

    /// @return total number of account details matching the query.
    size_t totalNumber() const;

    /// @return next page starting record, if this page is not last.
    boost::optional<const AccountDetailRecordId &> nextRecordId() const;

    std::string toString() const override;

    bool operator==(const ModelType &rhs) const override;
  };
}  // namespace shared_model
#endif  // IROHA_ACCOUNT_DETAIL_RESPONSE_HPP
