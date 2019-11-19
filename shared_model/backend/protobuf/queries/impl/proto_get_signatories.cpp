/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "backend/protobuf/queries/proto_get_signatories.hpp"

GetSignatories::GetSignatories(iroha::protocol::Query &query)
    : account_signatories_{query.payload().get_signatories()} {}

const types::AccountIdType &GetSignatories::accountId() const {
  return account_signatories_.account_id();
}
