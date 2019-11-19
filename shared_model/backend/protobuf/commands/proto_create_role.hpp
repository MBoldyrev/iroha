/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef IROHA_PROTO_CREATE_ROLE_HPP
#define IROHA_PROTO_CREATE_ROLE_HPP

#include "interfaces/commands/create_role.hpp"

#include "commands.pb.h"
#include "interfaces/permissions.hpp"

namespace shared_model {
  class CreateRole final : public CreateRole {
   public:
    explicit CreateRole(iroha::protocol::Command &command);

    const types::RoleIdType &roleName() const override;

    const RolePermissionSet &rolePermissions() const override;

    std::string toString() const override;

   private:
    const iroha::protocol::CreateRole &create_role_;

    const RolePermissionSet role_permissions_;
  };
}  // namespace shared_model

#endif  // IROHA_PROTO_CREATE_ROLE_HPP
