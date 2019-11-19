/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_PROTO_GRANT_PERMISSION_HPP
#define IROHA_PROTO_GRANT_PERMISSION_HPP

#include "interfaces/commands/grant_permission.hpp"

#include "commands.pb.h"

namespace shared_model {

  class GrantPermission final : public GrantPermission {
   public:
    explicit GrantPermission(iroha::protocol::Command &command);

    const types::AccountIdType &accountId() const override;

    permissions::Grantable permissionName() const override;

    std::string toString() const override;

   private:
    const iroha::protocol::GrantPermission &grant_permission_;
  };

}  // namespace shared_model

#endif  // IROHA_PROTO_GRANT_PERMISSION_HPP
