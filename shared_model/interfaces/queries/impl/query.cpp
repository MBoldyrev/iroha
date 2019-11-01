/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "interfaces/queries/query_variant.hpp"

#include "common/variant_transform.hpp"
#include "interfaces/queries/get_account.hpp"
#include "interfaces/queries/get_account_asset_transactions.hpp"
#include "interfaces/queries/get_account_assets.hpp"
#include "interfaces/queries/get_account_detail.hpp"
#include "interfaces/queries/get_account_transactions.hpp"
#include "interfaces/queries/get_asset_info.hpp"
#include "interfaces/queries/get_block.hpp"
#include "interfaces/queries/get_peers.hpp"
#include "interfaces/queries/get_pending_transactions.hpp"
#include "interfaces/queries/get_role_permissions.hpp"
#include "interfaces/queries/get_roles.hpp"
#include "interfaces/queries/get_signatories.hpp"
#include "interfaces/queries/get_transactions.hpp"
#include "interfaces/queries/query_payload_meta.hpp"
#include "queries.pb.h"
#include "utils/visitor_apply_for_all.hpp"

using namespace shared_model;

using Variant = Query::QueryVariantType;
template Variant::~variant();
template Variant::variant(Variant &&);
template bool Variant::operator==(const Variant &) const;
template void Variant::destroy_content() noexcept;
template int Variant::which() const noexcept;
template void Variant::indicate_which(int) noexcept;
template bool Variant::using_backup() const noexcept;
template Variant::convert_copy_into::convert_copy_into(void *) noexcept;

using PbQuery = iroha::protocol::Query;

namespace {
  using QueryUniquePointerVariant =
      iroha::TransformedVariant<Query::QueryVariantType,
                                iroha::metafunctions::ConstrefToUniquePointer>;

  QueryUniquePointerVariant loadQuery(PbQuery &pb_query) {
    switch (pb_query.mutable_payload().query_case()) {
      case PbQuery::Payload::kGetAccount:
        return std::make_unique<GetAccount>(pb_query);
      case PbQuery::Payload:::
        return std::make_unique<>(pb_query);
      case PbQuery::Payload::kGetSignatories:
        return std::make_unique<GetSignatories>(pb_query);
      case PbQuery::Payload::kGetAccountTransactions:
        return std::make_unique<GetAccountTransactions>(pb_query);
      case PbQuery::Payload::kGetAccountAssetTransactions:
        return std::make_unique<GetAccountAssetTransactions>(pb_query);
      case PbQuery::Payload::kGetTransactions:
        return std::make_unique<GetTransactions>(pb_query);
      case PbQuery::Payload::kGetAccountAssets:
        return std::make_unique<GetAccountAssets>(pb_query);
      case PbQuery::Payload::kGetAccountDetail:
        return std::make_unique<GetAccountDetail>(pb_query);
      case PbQuery::Payload::kGetRoles:
        return std::make_unique<GetRoles>(pb_query);
      case PbQuery::Payload::kGetRolePermissions:
        return std::make_unique<GetRolePermissions>(pb_query);
      case PbQuery::Payload::kGetAssetInfo:
        return std::make_unique<GetAssetInfo>(pb_query);
      case PbQuery::Payload::kGetPendingTransactions:
        return std::make_unique<GetPendingTransactions>(pb_query);
      case PbQuery::Payload::kGetBlock:
        return std::make_unique<GetBlock>(pb_query);
      case PbQuery::Payload::kGetPeers:
        return std::make_unique<GetPeers>(pb_query);
      default:
        BOOST_ASSERT_MSG(false, "unknown query");
        return {};
    };
  }
}  // namespace

struct Query::Impl {
  Impl(TransportType &ref)
      : query_holder_(loadQuery(ref)),
        query_constref_(boost::apply_visitor(
            iroha::indirecting_visitor<QueryVariantType>, query_holder_)) {}

  QueryUniquePointerVariant query_holder_;
  QueryVariantType query_constref_;
};

Query::Query(TransportType &ref) : impl_(std::make_unique<Impl>(ref)) {}

Query::~Query() = default;

const Query::QueryVariantType &Query::get() const {
  return impl_->query_constref_;
}

std::string Query::toString() const {
  return detail::PrettyStringBuilder()
      .init("Query")
      .append("creatorId", creatorAccountId())
      .append("queryCounter", std::to_string(queryCounter()))
      .append(Signable::toString())
      .append(boost::apply_visitor(detail::ToStringVisitor(), get()))
      .finalize();
}

bool Query::operator==(const ModelType &rhs) const {
  return this->get() == rhs.get();
}
