/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "interfaces/commands/append_role.hpp"

std::string AppendRole::toString() const {
  return detail::PrettyStringBuilder()
      .init("AppendRole")
      .append("role_name", roleName())
      .append("account_id", accountId())
      .finalize();
}

bool AppendRole::operator==(const ModelType &rhs) const {
  return accountId() == rhs.accountId() and roleName() == rhs.roleName();
}
