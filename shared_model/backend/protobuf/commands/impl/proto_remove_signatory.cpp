/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "backend/protobuf/commands/proto_remove_signatory.hpp"

#include "cryptography/hash.hpp"

RemoveSignatory::RemoveSignatory(iroha::protocol::Command &command)
    : remove_signatory_{command.remove_signatory()},
      pubkey_{crypto::Hash::fromHexString(remove_signatory_.public_key())} {}

const types::AccountIdType &RemoveSignatory::accountId() const {
  return remove_signatory_.account_id();
}

const types::PubkeyType &RemoveSignatory::pubkey() const {
  return pubkey_;
}
