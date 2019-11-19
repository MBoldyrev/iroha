/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "backend/protobuf/commands/proto_set_account_detail.hpp"

SetAccountDetail::SetAccountDetail(iroha::protocol::Command &command)
    : set_account_detail_{command.set_account_detail()} {}

const types::AccountIdType &SetAccountDetail::accountId() const {
  return set_account_detail_.account_id();
}

const types::AccountDetailKeyType &SetAccountDetail::key() const {
  return set_account_detail_.key();
}

const types::AccountDetailValueType &SetAccountDetail::value() const {
  return set_account_detail_.value();
}
