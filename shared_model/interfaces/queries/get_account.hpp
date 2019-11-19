/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_GET_ACCOUNT_HPP
#define IROHA_SHARED_MODEL_GET_ACCOUNT_HPP

#include "interfaces/base/model_primitive.hpp"
#include "interfaces/common_objects/types.hpp"

namespace shared_model {
  class GetAccount : public ModelPrimitive<GetAccount> {
   public:
    /**
     * @return Identity of user, for fetching data
     */
    const types::AccountIdType &accountId() const;

    std::string toString() const override;

    bool operator==(const ModelType &rhs) const override;
  };
}  // namespace shared_model
#endif  // IROHA_SHARED_MODEL_GET_ACCOUNT_HPP
