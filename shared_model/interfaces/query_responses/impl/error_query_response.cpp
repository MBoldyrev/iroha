/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "interfaces/query_responses/error_query_response.hpp"

#include "utils/visitor_apply_for_all.hpp"

using namespace shared_model;

std::string ErrorQueryResponse::toString() const {
  return detail::PrettyStringBuilder()
      .init("ErrorQueryResponse")
      .append(boost::apply_visitor(detail::ToStringVisitor(), get()))
      .append("errorMessage", errorMessage())
      .finalize();
}

bool ErrorQueryResponse::operator==(const ModelType &rhs) const {
  return get() == rhs.get();
}
