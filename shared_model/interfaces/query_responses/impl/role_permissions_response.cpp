/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "interfaces/query_responses/role_permissions_response.hpp"
#include "utils/string_builder.hpp"

#include "backend/protobuf/permissions.hpp"

using namespace shared_model;

RolePermissionsResponse::RolePermissionsResponse(
    iroha::protocol::QueryResponse &query_response)
    : role_permissions_response_{query_response.role_permissions_response()},
      role_permissions_{[&query_response] {
        auto &perms_in =
            query_response.role_permissions_response().permissions();
        RolePermissionSet perms_out;
        for (const auto &perm : perms_in) {
          perms_out.set(permissions::fromTransport(
              static_cast<iroha::protocol::RolePermission>(perm)));
        }
        return perms_out;
      }()} {}

const RolePermissionSet &RolePermissionsResponse::rolePermissions() const {
  return role_permissions_;
}

std::vector<std::string> RolePermissionsResponse::permissionsToString() const {
  return permissions::toString(rolePermissions());
}

bool RolePermissionsResponse::operator==(const ModelType &rhs) const {
  return rolePermissions() == rhs.rolePermissions();
}
