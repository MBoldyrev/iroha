/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "interfaces/commands/grant_permission.hpp"

bool GrantPermission::operator==(const ModelType &rhs) const {
  return accountId() == rhs.accountId()
      and permissionName() == rhs.permissionName();
}
