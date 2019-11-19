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

namespace {
  /// type of proto variant
  using ProtoQueryResponseVariantType =
      boost::variant<shared_model::AccountAssetResponse,
                     shared_model::AccountDetailResponse,
                     shared_model::AccountResponse,
                     shared_model::ErrorQueryResponse,
                     shared_model::SignatoriesResponse,
                     shared_model::TransactionsResponse,
                     shared_model::AssetResponse,
                     shared_model::RolesResponse,
                     shared_model::RolePermissionsResponse,
                     shared_model::TransactionsPageResponse,
                     shared_model::PendingTransactionsPageResponse,
                     shared_model::BlockResponse,
                     shared_model::PeersResponse>;

  /// list of types in variant
  using ProtoQueryResponseListType = ProtoQueryResponseVariantType::types;
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
