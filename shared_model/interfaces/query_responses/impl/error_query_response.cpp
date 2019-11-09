/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "interfaces/query_responses/error_query_response.hpp"

#include <unordered_map>

using namespace shared_model;

namespace {
  const std::unordered_map<QueryErrorType, std::string> kReasonToString{
      {kStatelessFailed, "StatelessFailed"},
      {kStatefulFailed, "StatefulFailed"},
      {kNoAccount, "NoAccount"},
      {kNoAccountAssets, "NoAccountAssets"},
      {kNoAccountDetail, "NoAccountDetail"},
      {kNoSignatories, "NoSignatories"},
      {kNotSupported, "NotSupported"},
      {kNoAsset, "NoAsset"},
      {kNoRoles "NoRoles"}};

  const std::string &reasonToString(QueryErrorType reason) {
    auto it = kReasonToString.find(reason);
    if (it == kReasonToString.end()) {
      assert(false);
      return "UnknownErrorType";
    }
    return it->second;
  }

  using ProtoQueryErrorType = iroha::protocol::ErrorResponse;
  // clang-format off
  const std::unordered_map<ProtoQueryErrorType, QueryErrorType>
      kProtoQueryErrorTypeToErrorQueryType{
    {ProtoQueryErrorType::STATELESS_INVALID,  QueryErrorType::kStatelessFailed},
    {ProtoQueryErrorType::STATEFUL_INVALID,   QueryErrorType::kStatefulFailed},
    {ProtoQueryErrorType::NO_ACCOUNT,         QueryErrorType::kNoAccount},
    {ProtoQueryErrorType::NO_ACCOUNT_ASSETS,  QueryErrorType::kNoAccountAssets},
    {ProtoQueryErrorType::NO_ACCOUNT_DETAIL,  QueryErrorType::kNoAccountDetail},
    {ProtoQueryErrorType::NO_SIGNATORIES,     QueryErrorType::kNoSignatories},
    {ProtoQueryErrorType::NOT_SUPPORTED,      QueryErrorType::kNotSupported},
    {ProtoQueryErrorType::NO_ASSET,           QueryErrorType::kNoAsset},
    {ProtoQueryErrorType::NO_ROLES,           QueryErrorType::kNoRoles}
  };
  // clang-format on

  QueryErrorType protoQueryErrorTypeToErrorQueryType(ProtoQueryErrorType type) {
    auto it = kProtoQueryErrorTypeToErrorQueryType.find(type);
    if (it == kProtoQueryErrorTypeToErrorQueryType.end()) {
      assert(false);
      return QueryErrorType::kStatefulFailed;
    }
    return it->second;
  }
}  // namespace

ErrorQueryResponse::ErrorQueryResponse(iroha::protocol::QueryResponse &ref)
    : error_response_{*ref.mutable_error_response()},
      error_reason_(
          protoQueryErrorTypeToErrorQueryType(error_response_.reason())),
      error_message_(error_response_.message()),
      error_code_(error_response_.error_code()) {}

const ErrorQueryResponse::ErrorMessageType &ErrorQueryResponse::reason() const {
  return reason_;
}

const ErrorQueryResponse::ErrorMessageType &ErrorQueryResponse::errorMessage()
    const {
  return error_message_;
}

QueryErrorCodeType ErrorQueryResponse::errorCode() const {
  return error_code_;
}

std::string ErrorQueryResponse::toString() const {
  return detail::PrettyStringBuilder()
      .init("ErrorQueryResponse")
      .append(reasonToString(reason()))
      .append("errorMessage", errorMessage())
      .finalize();
}

bool ErrorQueryResponse::operator==(const ModelType &rhs) const {
  return get() == rhs.get();
}
