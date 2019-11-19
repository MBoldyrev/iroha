/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_REVOKE_PERMISSION_HPP
#define IROHA_SHARED_MODEL_REVOKE_PERMISSION_HPP

#include "interfaces/base/model_primitive.hpp"

#include "commands.pb.h"

#include "interfaces/common_objects/types.hpp"
#include "interfaces/permissions.hpp"

namespace shared_model {
  /**
   * Revoke permission from account
   */
  class RevokePermission : public ModelPrimitive<RevokePermission> {
   public:
    explicit RevokePermission(iroha::protocol::Command &command);

    /**
     * @return account from which revoke permission
     */
    const types::AccountIdType &accountId() const;

    /**
     * @return Permission to revoke
     */
    permissions::Grantable permissionName() const;

    std::string toString() const override;

    bool operator==(const ModelType &rhs) const override;

   private:
    const iroha::protocol::RevokePermission &revoke_permission_;
  };
}  // namespace shared_model

#endif  // IROHA_SHARED_MODEL_REVOKE_PERMISSION_HPP
