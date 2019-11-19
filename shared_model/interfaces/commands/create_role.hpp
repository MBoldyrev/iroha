/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_CREATE_ROLE_HPP
#define IROHA_SHARED_MODEL_CREATE_ROLE_HPP

#include "interfaces/base/model_primitive.hpp"

#include "commands.pb.h"
#include "interfaces/permissions.hpp"

#include "interfaces/common_objects/types.hpp"
#include "interfaces/permissions.hpp"

namespace shared_model {
  /**
   * Create new role in Iroha
   */
  class CreateRole : public ModelPrimitive<CreateRole> {
   public:
    explicit CreateRole(iroha::protocol::Command &command);

    /**
     * @return Id of the domain to create
     */
    const types::RoleIdType &roleName() const;

    /**
     * @return permissions associated with the role
     */
    const RolePermissionSet &rolePermissions() const;

    std::string toString() const override;

    bool operator==(const ModelType &rhs) const override;

   private:
    const iroha::protocol::CreateRole &create_role_;

    const RolePermissionSet role_permissions_;
  };
}  // namespace shared_model

#endif  // IROHA_SHARED_MODEL_CREATE_ROLE_HPP
