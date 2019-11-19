/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "interfaces/proto_permission_to_string.hpp"

#include "interfaces/permissions.hpp"
#include "primitive.pb.h"

std::string ProtoPermissionToString::toString(permissions::Role r) {
  return iroha::protocol::RolePermission_Name(permissions::toTransport(r));
}

std::string ProtoPermissionToString::toString(permissions::Grantable r) {
  return iroha::protocol::GrantablePermission_Name(permissions::toTransport(r));
}

std::vector<std::string> ProtoPermissionToString::toString(
    const RolePermissionSet &set) {
  std::vector<std::string> v;
  for (size_t i = 0; i < set.size(); ++i) {
    auto perm = static_cast<permissions::Role>(i);
    if (set.isSet(perm)) {
      v.push_back(toString(perm));
    }
  }
  return v;
}

std::vector<std::string> ProtoPermissionToString::toString(
    const GrantablePermissionSet &set) {
  std::vector<std::string> v;
  for (size_t i = 0; i < set.size(); ++i) {
    auto perm = static_cast<permissions::Grantable>(i);
    if (set.isSet(perm)) {
      v.push_back(toString(perm));
    }
  }
  return v;
}
