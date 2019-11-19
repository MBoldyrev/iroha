/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "backend/protobuf/queries/proto_get_account.hpp"

GetAccount::GetAccount(iroha::protocol::Query &query)
    : account_{query.payload().get_account()} {}

const interface::types::AccountIdType &GetAccount::accountId() const {
  return account_.account_id();
}
