/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "backend/protobuf/query_responses/proto_account_response.hpp"

#include <boost/range/adaptor/transformed.hpp>

AccountResponse::AccountResponse(iroha::protocol::QueryResponse &query_response)
    : account_response_{query_response.account_response()},
      account_roles_{boost::copy_range<AccountRolesIdType>(
          account_response_.account_roles()
          | boost::adaptors::transformed(
                [](const auto &role) { return types::RoleIdType(role); }))},
      account_{*query_response.mutable_account_response()->mutable_account()} {}

const Account &AccountResponse::account() const {
  return account_;
}

const AccountResponse::AccountRolesIdType &AccountResponse::roles() const {
  return account_roles_;
}
