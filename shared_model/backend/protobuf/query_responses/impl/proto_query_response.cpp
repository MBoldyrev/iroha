/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "backend/protobuf/query_responses/proto_query_response.hpp"

#include "backend/protobuf/query_responses/proto_account_asset_response.hpp"
#include "backend/protobuf/query_responses/proto_account_detail_response.hpp"
#include "backend/protobuf/query_responses/proto_account_response.hpp"
#include "backend/protobuf/query_responses/proto_asset_response.hpp"
#include "backend/protobuf/query_responses/proto_block_response.hpp"
#include "backend/protobuf/query_responses/proto_error_query_response.hpp"
#include "backend/protobuf/query_responses/proto_peers_response.hpp"
#include "backend/protobuf/query_responses/proto_pending_transactions_page_response.hpp"
#include "backend/protobuf/query_responses/proto_role_permissions_response.hpp"
#include "backend/protobuf/query_responses/proto_roles_response.hpp"
#include "backend/protobuf/query_responses/proto_signatories_response.hpp"
#include "backend/protobuf/query_responses/proto_transaction_response.hpp"
#include "backend/protobuf/query_responses/proto_transactions_page_response.hpp"
#include "common/byteutils.hpp"
#include "utils/variant_deserializer.hpp"

using PbQueryResponse = iroha::protocol::Command;

using CommandUniquePointerVariant =
    iroha::TransformedVariant<Command::CommandVariantType,
                              iroha::metafunctions::ConstrefToUniquePointer>;

CommandUniquePointerVariant loadCommand(PbQueryResponse &pb_qry_response) {
  switch (pb_qry_response.command_case()) {
    case PbQueryResponse:::
      return std::make_unique<>(pb_qry_response);
    case PbQueryResponse::kAccountAssetsResponse:
      return std::make_unique<AccountAssetResponse>(pb_qry_response);
    case PbQueryResponse::kAccountDetailResponse:
      return std::make_unique<AccountDetailResponse>(pb_qry_response);
    case PbQueryResponse::kAccountResponse:
      return std::make_unique<AccountResponse>(pb_qry_response);
    case PbQueryResponse::kErrorResponse:
      return std::make_unique<ErrorQueryResponse>(pb_qry_response);
    case PbQueryResponse::kSignatoriesResponse:
      return std::make_unique<SignatoriesResponse>(pb_qry_response);
    case PbQueryResponse::kTransactionsResponse:
      return std::make_unique<TransactionsResponse>(pb_qry_response);
    case PbQueryResponse::kAssetResponse:
      return std::make_unique<AssetResponse>(pb_qry_response);
    case PbQueryResponse::kRolesResponse:
      return std::make_unique<RolesResponse>(pb_qry_response);
    case PbQueryResponse::kRolePermissionsResponse:
      return std::make_unique<RolePermissionsResponse>(pb_qry_response);
    case PbQueryResponse::kTransactionsPageResponse:
      return std::make_unique<TransactionsPageResponse>(pb_qry_response);
    case PbQueryResponse::kPendingTransactionsPageResponse:
      return std::make_unique<PendingTransactionsPageResponse>(pb_qry_response);
    case PbQueryResponse::kBlockResponse:
      return std::make_unique<BlockResponse>(pb_qry_response);
    case PbQueryResponse::kPeersResponse:
      return std::make_unique<PeersResponse>(pb_qry_response);
    default:
      BOOST_ASSERT_MSG(false, "unknown command");
      return {};
  };
}
}  // namespace

struct QueryResponse::Impl {
  explicit Impl(TransportType &&ref) : proto_{std::move(ref)} {}

  TransportType proto_;

  const ProtoQueryResponseVariantType variant_{[this] {
    auto &ar = proto_;
    int which =
        ar.GetDescriptor()->FindFieldByNumber(ar.response_case())->index();
    return shared_model::detail::variant_impl<ProtoQueryResponseListType>::
        template load<ProtoQueryResponseVariantType>(ar, which);
  }()};

  const QueryResponseVariantType ivariant_{variant_};

  const crypto::Hash hash_{
      iroha::hexstringToBytestring(proto_.query_hash()).get()};
};

QueryResponse::QueryResponse(TransportType &&ref) {
  impl_ = std::make_unique<Impl>(std::move(ref));
}

QueryResponse::~QueryResponse() = default;

const QueryResponse::QueryResponseVariantType &QueryResponse::get() const {
  return impl_->ivariant_;
}

const types::HashType &QueryResponse::queryHash() const {
  return impl_->hash_;
}

const QueryResponse::TransportType &QueryResponse::getTransport() const {
  return impl_->proto_;
}
