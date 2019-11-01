/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "interfaces/commands/command_variant.hpp"

#include "commands.pb.h"
#include "common/variant_transform.hpp"
#include "interfaces/commands/add_asset_quantity.hpp"
#include "interfaces/commands/add_peer.hpp"
#include "interfaces/commands/add_signatory.hpp"
#include "interfaces/commands/append_role.hpp"
#include "interfaces/commands/compare_and_set_account_detail.hpp"
#include "interfaces/commands/create_account.hpp"
#include "interfaces/commands/create_asset.hpp"
#include "interfaces/commands/create_domain.hpp"
#include "interfaces/commands/create_role.hpp"
#include "interfaces/commands/detach_role.hpp"
#include "interfaces/commands/grant_permission.hpp"
#include "interfaces/commands/remove_peer.hpp"
#include "interfaces/commands/remove_signatory.hpp"
#include "interfaces/commands/revoke_permission.hpp"
#include "interfaces/commands/set_account_detail.hpp"
#include "interfaces/commands/set_quorum.hpp"
#include "interfaces/commands/set_setting_value.hpp"
#include "interfaces/commands/subtract_asset_quantity.hpp"
#include "interfaces/commands/transfer_asset.hpp"
#include "utils/visitor_apply_for_all.hpp"

using namespace shared_model;

using PbCommand = iroha::protocol::Command;

using Variant = Command::CommandVariantType;
template Variant::~variant();
template Variant::variant(Variant &&);
template bool Variant::operator==(const Variant &) const;
template void Variant::destroy_content() noexcept;
template int Variant::which() const noexcept;
template void Variant::indicate_which(int) noexcept;
template bool Variant::using_backup() const noexcept;
template Variant::convert_copy_into::convert_copy_into(void *) noexcept;

namespace {
  using CommandUniquePointerVariant =
      iroha::TransformedVariant<Command::CommandVariantType,
                                iroha::metafunctions::ConstrefToUniquePointer>;

  CommandUniquePointerVariant loadCommand(PbCommand &pb_command) {
    switch (pb_command.command_case()) {
      case PbCommand::kAddAssetQuantity:
        return std::make_unique<AddAssetQuantity>(pb_command);
      case PbCommand::kAddPeer:
        return std::make_unique<AddPeer>(pb_command);
      case PbCommand::kAddSignatory:
        return std::make_unique<AddSignatory>(pb_command);
      case PbCommand::kAppendRole:
        return std::make_unique<AppendRole>(pb_command);
      case PbCommand::kCreateAccount:
        return std::make_unique<CreateAccount>(pb_command);
      case PbCommand::kCreateAsset:
        return std::make_unique<CreateAsset>(pb_command);
      case PbCommand::kCreateDomain:
        return std::make_unique<CreateDomain>(pb_command);
      case PbCommand::kCreateRole:
        return std::make_unique<CreateRole>(pb_command);
      case PbCommand::kDetachRole:
        return std::make_unique<DetachRole>(pb_command);
      case PbCommand::kGrantPermission:
        return std::make_unique<GrantPermission>(pb_command);
      case PbCommand::kRemoveSignatory:
        return std::make_unique<RemoveSignatory>(pb_command);
      case PbCommand::kRevokePermission:
        return std::make_unique<RevokePermission>(pb_command);
      case PbCommand::kSetAccountDetail:
        return std::make_unique<SetAccountDetail>(pb_command);
      case PbCommand::kSetAccountQuorum:
        return std::make_unique<SetQuorum>(pb_command);
      case PbCommand::kSubtractAssetQuantity:
        return std::make_unique<SubtractAssetQuantity>(pb_command);
      case PbCommand::kTransferAsset:
        return std::make_unique<TransferAsset>(pb_command);
      case PbCommand::kRemovePeer:
        return std::make_unique<RemovePeer>(pb_command);
      case PbCommand::kCompareAndSetAccountDetail:
        return std::make_unique<CompareAndSetAccountDetail>(pb_command);
      case PbCommand::kSetSettingValue:
        return std::make_unique<SetSettingValue>(pb_command);
      default:
        BOOST_ASSERT_MSG(false, "unknown command");
        return {};
    };
  }
}  // namespace

struct Command::Impl {
  Impl(TransportType &ref)
      : command_holder_(loadCommand(ref)),
        command_constref_(boost::apply_visitor(
            iroha::indirecting_visitor<CommandVariantType>, command_holder_)) {}

  CommandUniquePointerVariant command_holder_;
  CommandVariantType command_constref_;
};

Command::Command(TransportType &ref) : impl_(std::make_unique<Impl>(ref)) {}

Command::~Command() = default;

const Command::CommandVariantType &Command::get() const {
  return impl_->command_constref_;
}

std::string Command::toString() const {
  return boost::apply_visitor(detail::ToStringVisitor(), get());
}

bool Command::operator==(const ModelType &rhs) const {
  return this->get() == rhs.get();
}
