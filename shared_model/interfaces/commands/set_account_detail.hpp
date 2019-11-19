/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_SET_ACCOUNT_DETAIL_HPP
#define IROHA_SHARED_MODEL_SET_ACCOUNT_DETAIL_HPP

#include "interfaces/base/model_primitive.hpp"

#include "commands.pb.h"

#include "interfaces/common_objects/types.hpp"

namespace shared_model {

  /**
   * Set key-value pair of given account
   */
  class SetAccountDetail : public ModelPrimitive<SetAccountDetail> {
   public:
    explicit SetAccountDetail(iroha::protocol::Command &command);

    /**
     * @return Identity of user to set account detail
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

    std::string toString() const override;

    bool operator==(const ModelType &rhs) const override;

   private:
    const iroha::protocol::SetAccountDetail &set_account_detail_;
  };
}  // namespace shared_model

#endif  // IROHA_SHARED_MODEL_SET_ACCOUNT_DETAIL_HPP
