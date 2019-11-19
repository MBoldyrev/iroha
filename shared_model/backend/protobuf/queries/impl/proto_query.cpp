/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "backend/protobuf/queries/proto_query.hpp"

#include "backend/protobuf/common_objects/signature.hpp"
#include "backend/protobuf/queries/proto_get_account.hpp"
#include "backend/protobuf/queries/proto_get_account_asset_transactions.hpp"
#include "backend/protobuf/queries/proto_get_account_assets.hpp"
#include "backend/protobuf/queries/proto_get_account_detail.hpp"
#include "backend/protobuf/queries/proto_get_account_transactions.hpp"
#include "backend/protobuf/queries/proto_get_asset_info.hpp"
#include "backend/protobuf/queries/proto_get_block.hpp"
#include "backend/protobuf/queries/proto_get_peers.hpp"
#include "backend/protobuf/queries/proto_get_pending_transactions.hpp"
#include "backend/protobuf/queries/proto_get_role_permissions.hpp"
#include "backend/protobuf/queries/proto_get_roles.hpp"
#include "backend/protobuf/queries/proto_get_signatories.hpp"
#include "backend/protobuf/queries/proto_get_transactions.hpp"
#include "backend/protobuf/util.hpp"
#include "utils/variant_deserializer.hpp"

namespace {
  /// type of proto variant
  using ProtoQueryVariantType =
      boost::variant<shared_model::GetAccount,
                     shared_model::GetSignatories,
                     shared_model::GetAccountTransactions,
                     shared_model::GetAccountAssetTransactions,
                     shared_model::GetTransactions,
                     shared_model::GetAccountAssets,
                     shared_model::GetAccountDetail,
                     shared_model::GetRoles,
                     shared_model::GetRolePermissions,
                     shared_model::GetAssetInfo,
                     shared_model::GetPendingTransactions,
                     shared_model::GetBlock,
                     shared_model::GetPeers>;

  /// list of types in proto variant
  using ProtoQueryListType = ProtoQueryVariantType::types;
}  // namespace

struct Query::Impl {
  explicit Impl(TransportType &&ref) : proto_{std::move(ref)} {}
  explicit Impl(const TransportType &ref) : proto_{ref} {}

  TransportType proto_;

  ProtoQueryVariantType variant_{[this] {
    auto &ar = proto_;
    int which = ar.payload()
                    .GetDescriptor()
                    ->FindFieldByNumber(ar.payload().query_case())
                    ->index_in_oneof();
    return shared_model::detail::variant_impl<
        ProtoQueryListType>::template load<ProtoQueryVariantType>(ar, which);
  }()};

  QueryVariantType ivariant_{variant_};

  types::BlobType blob_{makeBlob(proto_)};

  types::BlobType payload_{makeBlob(proto_.payload())};

  SignatureSetType<Signature> signatures_{[this] {
    SignatureSetType<Signature> set;
    if (proto_.has_signature()) {
      set.emplace(*proto_.mutable_signature());
    }
    return set;
  }()};

  types::HashType hash_ = makeHash(payload_);
};

Query::Query(const Query &o) : Query(o.impl_->proto_) {}
Query::Query(Query &&o) noexcept = default;

Query::Query(const TransportType &ref) {
  impl_ = std::make_unique<Impl>(ref);
}
Query::Query(TransportType &&ref) {
  impl_ = std::make_unique<Impl>(std::move(ref));
}

Query::~Query() = default;

const Query::QueryVariantType &Query::get() const {
  return impl_->ivariant_;
}

const types::AccountIdType &Query::creatorAccountId() const {
  return impl_->proto_.payload().meta().creator_account_id();
}

types::CounterType Query::queryCounter() const {
  return impl_->proto_.payload().meta().query_counter();
}

const types::BlobType &Query::blob() const {
  return impl_->blob_;
}

const types::BlobType &Query::payload() const {
  return impl_->payload_;
}

types::SignatureRangeType Query::signatures() const {
  return impl_->signatures_;
}

bool Query::addSignature(const crypto::Signed &signed_blob,
                         const crypto::PublicKey &public_key) {
  if (impl_->proto_.has_signature()) {
    return false;
  }

  auto sig = impl_->proto_.mutable_signature();
  sig->set_signature(signed_blob.hex());
  sig->set_public_key(public_key.hex());

  impl_->signatures_ = SignatureSetType<Signature>{Signature{*sig}};
  return true;
}

const types::HashType &Query::hash() const {
  return impl_->hash_;
}

types::TimestampType Query::createdTime() const {
  return impl_->proto_.payload().meta().created_time();
}

const Query::TransportType &Query::getTransport() const {
  return impl_->proto_;
}
