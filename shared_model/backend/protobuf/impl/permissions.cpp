/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "backend/protobuf/permissions.hpp"

namespace shared_model {

  permissions::Role fromTransport(
      iroha::protocol::RolePermission perm) noexcept {
    return static_cast<permissions::Role>(perm);
  }

  iroha::protocol::RolePermission toTransport(permissions::Role r) {
    return static_cast<iroha::protocol::RolePermission>(r);
  }

  std::string toString(permissions::Role r) {
    return iroha::protocol::RolePermission_Name(toTransport(r));
  }

  permissions::Grantable fromTransport(
      iroha::protocol::GrantablePermission perm) noexcept {
    return static_cast<permissions::Grantable>(perm);
  }

  iroha::protocol::GrantablePermission toTransport(permissions::Grantable r) {
    return static_cast<iroha::protocol::GrantablePermission>(r);
  }

  std::string toString(permissions::Grantable r) {
    return iroha::protocol::GrantablePermission_Name(toTransport(r));
  }

  std::vector<std::string> toString(
      const PermissionSet<permissions::Role> &set) {
    std::vector<std::string> v;
    for (size_t i = 0; i < set.size(); ++i) {
      auto perm = static_cast<permissions::Role>(i);
      if (set.isSet(perm)) {
        v.push_back(toString(perm));
      }
    }
    return v;
  }

  std::vector<std::string> toString(
      const PermissionSet<permissions::Grantable> &set) {
    std::vector<std::string> v;
    for (size_t i = 0; i < set.size(); ++i) {
      auto perm = static_cast<permissions::Grantable>(i);
      if (set.isSet(perm)) {
        v.push_back(toString(perm));
      }
    }
    return v;
  }
}  // namespace shared_model
