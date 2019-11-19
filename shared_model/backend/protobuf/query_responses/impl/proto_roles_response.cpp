/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "backend/protobuf/query_responses/proto_roles_response.hpp"

#include <boost/range/iterator_range_core.hpp>

RolesResponse::RolesResponse(iroha::protocol::QueryResponse &query_response)
    : roles_response_{query_response.roles_response()},
      roles_{boost::copy_range<RolesIdType>(roles_response_.roles())} {}

const RolesResponse::RolesIdType &RolesResponse::roles() const {
  return roles_;
}
