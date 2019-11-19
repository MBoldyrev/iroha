/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_PROTO_GET_ROLE_PERMISSIONS_H
#define IROHA_PROTO_GET_ROLE_PERMISSIONS_H

#include "interfaces/queries/get_role_permissions.hpp"

#include "queries.pb.h"

namespace shared_model {
  class GetRolePermissions final : public GetRolePermissions {
   public:
    explicit GetRolePermissions(iroha::protocol::Query &query);

    const types::RoleIdType &roleId() const override;

   private:
    // ------------------------------| fields |-------------------------------
    const iroha::protocol::GetRolePermissions &role_permissions_;
  };

}  // namespace shared_model

#endif  // IROHA_PROTO_GET_ROLE_PERMISSIONS_H
