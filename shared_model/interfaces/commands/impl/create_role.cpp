/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "interfaces/commands/create_role.hpp"

bool CreateRole::operator==(const ModelType &rhs) const {
  return roleName() == rhs.roleName()
      and rolePermissions() == rhs.rolePermissions();
}
