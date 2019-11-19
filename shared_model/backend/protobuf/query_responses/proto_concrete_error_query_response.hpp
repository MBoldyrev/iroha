/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_PROTO_CONCRETE_ERROR_QUERY_RESPONSE_HPP
#define IROHA_SHARED_MODEL_PROTO_CONCRETE_ERROR_QUERY_RESPONSE_HPP

#include "backend/protobuf/common_objects/proto_ref.hpp"
#include "interfaces/query_responses/error_responses/no_account_assets_error_response.hpp"
#include "interfaces/query_responses/error_responses/no_account_detail_error_response.hpp"
#include "interfaces/query_responses/error_responses/no_account_error_response.hpp"
#include "interfaces/query_responses/error_responses/no_asset_error_response.hpp"
#include "interfaces/query_responses/error_responses/no_roles_error_response.hpp"
#include "interfaces/query_responses/error_responses/no_signatories_error_response.hpp"
#include "interfaces/query_responses/error_responses/not_supported_error_response.hpp"
#include "interfaces/query_responses/error_responses/stateful_failed_error_response.hpp"
#include "interfaces/query_responses/error_responses/stateless_failed_error_response.hpp"
#include "qry_responses.pb.h"

namespace shared_model {
  using StatelessFailedErrorResponse =
      ProtoRef<StatelessFailedErrorResponse, iroha::protocol::ErrorResponse>;
  using StatefulFailedErrorResponse =
      ProtoRef<StatefulFailedErrorResponse, iroha::protocol::ErrorResponse>;
  using NoAccountErrorResponse =
      ProtoRef<NoAccountErrorResponse, iroha::protocol::ErrorResponse>;
  using NoAccountAssetsErrorResponse =
      ProtoRef<NoAccountAssetsErrorResponse, iroha::protocol::ErrorResponse>;
  using NoAccountDetailErrorResponse =
      ProtoRef<NoAccountDetailErrorResponse, iroha::protocol::ErrorResponse>;
  using NoSignatoriesErrorResponse =
      ProtoRef<NoSignatoriesErrorResponse, iroha::protocol::ErrorResponse>;
  using NotSupportedErrorResponse =
      ProtoRef<NotSupportedErrorResponse, iroha::protocol::ErrorResponse>;
  using NoAssetErrorResponse =
      ProtoRef<NoAssetErrorResponse, iroha::protocol::ErrorResponse>;
  using NoRolesErrorResponse =
      ProtoRef<NoRolesErrorResponse, iroha::protocol::ErrorResponse>;
}  // namespace shared_model

#endif  // IROHA_SHARED_MODEL_PROTO_CONCRETE_ERROR_QUERY_RESPONSE_HPP
