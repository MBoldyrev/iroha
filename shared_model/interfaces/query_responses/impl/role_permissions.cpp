/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "interfaces/query_responses/role_permissions.hpp"
#include "utils/string_builder.hpp"

using namespace shared_model;

bool RolePermissionsResponse::operator==(const ModelType &rhs) const {
  return rolePermissions() == rhs.rolePermissions();
}
