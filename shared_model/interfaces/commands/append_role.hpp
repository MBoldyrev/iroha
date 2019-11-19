/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_APPEND_ROLE_HPP
#define IROHA_SHARED_MODEL_APPEND_ROLE_HPP

#include "interfaces/base/model_primitive.hpp"

#include "commands.pb.h"

#include "interfaces/common_objects/types.hpp"

namespace shared_model {

  /**
   * Add role to account used in Iroha
   */
  class AppendRole : public ModelPrimitive<AppendRole> {
   public:
    explicit AppendRole(iroha::protocol::Command &command);

    /**
     * @return Account to add the role
     */
    const types::AccountIdType &accountId() const;
    /**
     * @return Role name to add to account
     */
    const types::RoleIdType &roleName() const;

    std::string toString() const override;

    bool operator==(const ModelType &rhs) const override;

   private:
    const iroha::protocol::AppendRole &append_role_;
  };
}  // namespace shared_model

#endif  // IROHA_SHARED_MODEL_APPEND_ROLE_HPP
