/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_COMPARE_AND_SET_ACCOUNT_DETAIL_HPP
#define IROHA_SHARED_MODEL_COMPARE_AND_SET_ACCOUNT_DETAIL_HPP

#include <boost/optional.hpp>

#include "commands.pb.h"
#include "interfaces/base/model_primitive.hpp"

#include "interfaces/common_objects/types.hpp"

namespace shared_model {

  /**
   * Set key-value pair of given account if the current value matches provided
   * expectation
   */
  class CompareAndSetAccountDetail
      : public ModelPrimitive<CompareAndSetAccountDetail> {
   public:
    explicit CompareAndSetAccountDetail(iroha::protocol::Command &command);

    /**
     * @return Identity of user to set account detail to
     */
    const types::AccountIdType &accountId() const;

    /**
     * @return key of data to store in the account
     */
    const types::AccountDetailKeyType &key() const;

    /**
     * @return detail value to store by given key
     */
    const types::AccountDetailValueType &value() const;

    /**
     * @return the value expected before the change
     */
    const boost::optional<types::AccountDetailValueType> oldValue() const;

    std::string toString() const override;

    bool operator==(const ModelType &rhs) const override;

   private:
    const iroha::protocol::CompareAndSetAccountDetail
        &compare_and_set_account_detail_;
  };
}  // namespace shared_model

#endif  // IROHA_SHARED_MODEL_COMPARE_AND_SET_ACCOUNT_DETAIL_HPP
