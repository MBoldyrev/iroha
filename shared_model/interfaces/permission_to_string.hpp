/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef PERMISSION_TO_STRING_HPP
#define PERMISSION_TO_STRING_HPP

#include <string>
#include <vector>

#include "interfaces/permissions.hpp"

namespace shared_model {
  class PermissionToString {
   public:
    ~PermissionToString() = default;
    /**
     * @param sm object for conversion
     * @return its string representation
     */
    std::string toString(permissions::Role r);

    /**
     * @param sm object for conversion
     * @return its string representation
     */
    std::string toString(permissions::Grantable r);

    /**
     * @param set for stringify
     * @return vector of string representation of set elements
     */
    std::vector<std::string> toString(const RolePermissionSet &set);

    /**
     * @param set for stringify
     * @return vector of string representation of set elements
     */
    std::vector<std::string> toString(const GrantablePermissionSet &set);
  };
}  // namespace shared_model

#endif  // PERMISSION_TO_STRING_HPP
