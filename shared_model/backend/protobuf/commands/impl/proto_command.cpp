/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "backend/protobuf/commands/proto_command.hpp"

#include "backend/protobuf/commands/proto_add_asset_quantity.hpp"
#include "backend/protobuf/commands/proto_add_peer.hpp"
#include "backend/protobuf/commands/proto_add_signatory.hpp"
#include "backend/protobuf/commands/proto_append_role.hpp"
#include "backend/protobuf/commands/proto_compare_and_set_account_detail.hpp"
#include "backend/protobuf/commands/proto_create_account.hpp"
#include "backend/protobuf/commands/proto_create_asset.hpp"
#include "backend/protobuf/commands/proto_create_domain.hpp"
#include "backend/protobuf/commands/proto_create_role.hpp"
#include "backend/protobuf/commands/proto_detach_role.hpp"
#include "backend/protobuf/commands/proto_grant_permission.hpp"
#include "backend/protobuf/commands/proto_remove_peer.hpp"
#include "backend/protobuf/commands/proto_remove_signatory.hpp"
#include "backend/protobuf/commands/proto_revoke_permission.hpp"
#include "backend/protobuf/commands/proto_set_account_detail.hpp"
#include "backend/protobuf/commands/proto_set_quorum.hpp"
#include "backend/protobuf/commands/proto_set_setting_value.hpp"
#include "backend/protobuf/commands/proto_subtract_asset_quantity.hpp"
#include "backend/protobuf/commands/proto_transfer_asset.hpp"
#include "utils/variant_deserializer.hpp"

namespace {
  /// type of proto variant
  using ProtoCommandVariantType =
      ::boost::variant<shared_model::AddAssetQuantity,
                       shared_model::AddPeer,
                       shared_model::AddSignatory,
                       shared_model::AppendRole,
                       shared_model::CreateAccount,
                       shared_model::CreateAsset,
                       shared_model::CreateDomain,
                       shared_model::CreateRole,
                       shared_model::DetachRole,
                       shared_model::GrantPermission,
                       shared_model::RemoveSignatory,
                       shared_model::RevokePermission,
                       shared_model::SetAccountDetail,
                       shared_model::SetQuorum,
                       shared_model::SubtractAssetQuantity,
                       shared_model::TransferAsset,
                       shared_model::RemovePeer,
                       shared_model::CompareAndSetAccountDetail,
                       shared_model::SetSettingValue>;

  /// list of types in proto variant
  using ProtoCommandListType = ProtoCommandVariantType::types;
}  // namespace

struct Command::Impl {
  explicit Impl(TransportType &ref) : proto_(ref) {}

  TransportType &proto_;

  ProtoCommandVariantType variant_{[this] {
    auto &ar = proto_;
    int which =
        ar.GetDescriptor()->FindFieldByNumber(ar.command_case())->index();
    return shared_model::detail::variant_impl<
        ProtoCommandListType>::template load<ProtoCommandVariantType>(ar,
                                                                      which);
  }()};

  CommandVariantType ivariant_{variant_};
};

Command::Command(TransportType &ref) {
  impl_ = std::make_unique<Impl>(ref);
}

Command::~Command() = default;

const Command::CommandVariantType &Command::get() const {
  return impl_->ivariant_;
}
