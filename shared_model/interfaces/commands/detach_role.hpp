/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_DETACH_ROLE_HPP
#define IROHA_SHARED_MODEL_DETACH_ROLE_HPP

#include "interfaces/base/model_primitive.hpp"

#include "interfaces/common_objects/types.hpp"

namespace shared_model {

  /**
   * Remove role from account used in Iroha
   */
  class DetachRole : public ModelPrimitive<DetachRole> {
   public:
    /**
     * @return Account to remove the role
     */
    const types::AccountIdType &accountId() const;
    /**
     * @return Role name to remove from account
     */
    const types::RoleIdType &roleName() const;

    std::string toString() const override;

    bool operator==(const ModelType &rhs) const override;
  };
}  // namespace shared_model

#endif  // IROHA_SHARED_MODEL_DETACH_ROLE_HPP
