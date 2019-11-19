/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "backend/protobuf/commands/proto_add_signatory.hpp"

#include "cryptography/hash.hpp"

AddSignatory::AddSignatory(iroha::protocol::Command &command)
    : add_signatory_{command.add_signatory()},
      pubkey_{crypto::Hash::fromHexString(add_signatory_.public_key())} {}

const types::AccountIdType &AddSignatory::accountId() const {
  return add_signatory_.account_id();
}

const types::PubkeyType &AddSignatory::pubkey() const {
  return pubkey_;
}
