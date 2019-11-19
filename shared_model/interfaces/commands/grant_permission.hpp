/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_GRANT_PERMISSION_HPP
#define IROHA_SHARED_MODEL_GRANT_PERMISSION_HPP

#include "interfaces/base/model_primitive.hpp"

#include "interfaces/common_objects/types.hpp"
#include "interfaces/permissions.hpp"

namespace shared_model {
  /**
   * Grant permission to the account
   */
  class GrantPermission : public ModelPrimitive<GrantPermission> {
   public:
    /**
     * @return Id of the account to whom grant permission
     */
    const types::AccountIdType &accountId() const;
    /**
     * @return permission to grant
     */
    permissions::Grantable permissionName() const;

    std::string toString() const override;

    bool operator==(const ModelType &rhs) const override;
  };
}  // namespace shared_model

#endif  // IROHA_SHARED_MODEL_GRANT_PERMISSIONS_HPP
