/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_PROTO_ROLE_PERMISSIONS_RESPONSE_HPP
#define IROHA_SHARED_MODEL_PROTO_ROLE_PERMISSIONS_RESPONSE_HPP

#include "interfaces/query_responses/role_permissions.hpp"

#include "qry_responses.pb.h"

namespace shared_model {
  class RolePermissionsResponse final : public RolePermissionsResponse {
   public:
    explicit RolePermissionsResponse(
        iroha::protocol::QueryResponse &query_response);

    const RolePermissionSet &rolePermissions() const override;

    std::string toString() const override;

   private:
    const iroha::protocol::RolePermissionsResponse &role_permissions_response_;

    const RolePermissionSet role_permissions_;
  };
}  // namespace shared_model

#endif  // IROHA_SHARED_MODEL_PROTO_ROLE_PERMISSIONS_RESPONSE_HPP
