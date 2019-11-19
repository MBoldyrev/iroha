/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "backend/protobuf/commands/proto_remove_peer.hpp"

RemovePeer::RemovePeer(iroha::protocol::Command &command)
    : pubkey_{crypto::Hash::fromHexString(command.remove_peer().public_key())} {
}

const interface::types::PubkeyType &RemovePeer::pubkey() const {
  return pubkey_;
}
