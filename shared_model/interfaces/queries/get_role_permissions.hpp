/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_GET_ROLE_PERMISSIONS_HPP
#define IROHA_SHARED_MODEL_GET_ROLE_PERMISSIONS_HPP

#include "interfaces/base/model_primitive.hpp"

#include "interfaces/common_objects/types.hpp"
#include "queries.pb.h"

namespace shared_model {

  /**
   * Get all permissions related to specific role
   */
  class GetRolePermissions : public ModelPrimitive<GetRolePermissions> {
   public:
    explicit GetRolePermissions(iroha::protocol::Query &query);

    /**
     * @return role identifier containing requested permissions
     */
    const types::RoleIdType &roleId() const;

    std::string toString() const override;

    bool operator==(const ModelType &rhs) const override;

   private:
    // ------------------------------| fields |-------------------------------
    const iroha::protocol::GetRolePermissions &role_permissions_;
  };
}  // namespace shared_model

#endif  // IROHA_SHARED_MODEL_GET_ROLES_HPP
