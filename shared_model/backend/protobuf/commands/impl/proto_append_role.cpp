/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "backend/protobuf/commands/proto_append_role.hpp"

AppendRole::AppendRole(iroha::protocol::Command &command)
    : append_role_{command.append_role()} {}

const interface::types::AccountIdType &AppendRole::accountId() const {
  return append_role_.account_id();
}

const interface::types::RoleIdType &AppendRole::roleName() const {
  return append_role_.role_name();
}
