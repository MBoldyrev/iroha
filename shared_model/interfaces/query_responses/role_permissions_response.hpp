/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_ROLE_PERMISSIONS_RESPONSE_HPP
#define IROHA_SHARED_MODEL_ROLE_PERMISSIONS_RESPONSE_HPP

#include "interfaces/base/model_primitive.hpp"

#include "interfaces/common_objects/types.hpp"
#include "interfaces/permissions.hpp"

namespace shared_model {
  /**
   * Response with all permissions related to role
   */
  class RolePermissionsResponse
      : public ModelPrimitive<RolePermissionsResponse> {
   public:
    explicit RolePermissionsResponse(
        iroha::protocol::QueryResponse &query_response);

    /**
     * @return role permissions
     */
    const RolePermissionSet &rolePermissions() const;

    /**
     * Stringify the data.
     * @return string representation of data.
     */
    std::string toString() const override;

    bool operator==(const ModelType &rhs) const override;

   private:
    const iroha::protocol::RolePermissionsResponse &role_permissions_response_;

    const RolePermissionSet role_permissions_;
  };
}  // namespace shared_model
#endif  // IROHA_SHARED_MODEL_ROLE_PERMISSIONS_RESPONSE_HPP
