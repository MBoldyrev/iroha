/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_PROTO_PERMISSIONS_HPP
#define IROHA_SHARED_MODEL_PROTO_PERMISSIONS_HPP

#include "interfaces/permissions.hpp"

#include <string>
#include <vector>

#include "primitive.pb.h"

namespace shared_model {
  namespace permissions {
    /**
     * Perform deserialization without checks
     * @param perm protocol object for conversion
     * @return sm object
     */
    permissions::Role fromTransport(
        iroha::protocol::RolePermission perm) noexcept;

    /**
     * @param sm object for conversion
     * @return protobuf object
     */
    iroha::protocol::RolePermission toTransport(permissions::Role r);

    /**
     * @param sm object for conversion
     * @return its string representation
     */
    std::string toString(permissions::Role r);

    /**
     * Perform deserialization without checks
     * @param perm protocol object for conversion
     * @return sm object
     */
    permissions::Grantable fromTransport(
        iroha::protocol::GrantablePermission perm) noexcept;

    /**
     * @param sm object for conversion
     * @return protobuf object
     */
    iroha::protocol::GrantablePermission toTransport(permissions::Grantable r);

    /**
     * @param sm object for conversion
     * @return its string representation
     */
    std::string toString(permissions::Grantable r);

    /**
     * @param set for stringify
     * @return vector of string representation of set elements
     */
    std::vector<std::string> toString(
        const PermissionSet<permissions::Role> &set);

    /**
     * @param set for stringify
     * @return vector of string representation of set elements
     */
    std::vector<std::string> toString(
        const PermissionSet<permissions::Grantable> &set);
  }  // namespace permissions
}  // namespace shared_model

#endif  // IROHA_SHARED_MODEL_PROTO_PERMISSIONS_HPP
