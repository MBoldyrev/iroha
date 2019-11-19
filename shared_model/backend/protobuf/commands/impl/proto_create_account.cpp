/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "backend/protobuf/commands/proto_create_account.hpp"

#include "cryptography/hash.hpp"

CreateAccount::CreateAccount(iroha::protocol::Command &command)
    : create_account_{command.create_account()},
      pubkey_{crypto::Hash::fromHexString(create_account_.public_key())} {}

const types::PubkeyType &CreateAccount::pubkey() const {
  return pubkey_;
}

const types::AccountNameType &CreateAccount::accountName() const {
  return create_account_.account_name();
}

const types::DomainIdType &CreateAccount::domainId() const {
  return create_account_.domain_id();
}
