/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "backend/protobuf/commands/proto_set_quorum.hpp"

SetQuorum::SetQuorum(iroha::protocol::Command &command)
    : set_account_quorum_{command.set_account_quorum()} {}

const types::AccountIdType &SetQuorum::accountId() const {
  return set_account_quorum_.account_id();
}

types::QuorumType SetQuorum::newQuorum() const {
  return set_account_quorum_.quorum();
}
