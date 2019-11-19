/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef PROTO_PERMISSION_TO_STRING_HPP
#define PROTO_PERMISSION_TO_STRING_HPP

#include "interfaces/permission_to_string.hpp"

namespace shared_model {
  class ProtoPermissionToString : public PermissionToString {
   public:
    std::string toString(permissions::Role r) override;
    std::string toString(permissions::Grantable r) override;

    std::vector<std::string> toString(const RolePermissionSet &set) override;
    std::vector<std::string> toString(
        const GrantablePermissionSet &set) override;
  };
}  // namespace shared_model

#endif  // PROTO_PERMISSION_TO_STRING_HPP
