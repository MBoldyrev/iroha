/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_WSV_COMMAND_IMPL_HPP
#define IROHA_WSV_COMMAND_IMPL_HPP

#include "ametsuchi/wsv_command.hpp"

namespace iroha {
  namespace newstorage {

    using ametsuchi::WsvCommandResult;
    class MutableWsv;

   class WsvCommandImpl : public ametsuchi::WsvCommand {
     public:
      explicit WsvCommandImpl(MutableWsv &db);

      WsvCommandResult insertRole(
          const shared_model::interface::types::RoleIdType &role_name) override;

      WsvCommandResult insertAccountRole(
          const shared_model::interface::types::AccountIdType &account_id,
          const shared_model::interface::types::RoleIdType &role_name) override;
      WsvCommandResult deleteAccountRole(
          const shared_model::interface::types::AccountIdType &account_id,
          const shared_model::interface::types::RoleIdType &role_name) override;

      WsvCommandResult insertRolePermissions(
          const shared_model::interface::types::RoleIdType &role_id,
          const shared_model::interface::RolePermissionSet &permissions)
          override;

      WsvCommandResult insertAccount(
          const shared_model::interface::Account &account) override;
      WsvCommandResult updateAccount(
          const shared_model::interface::Account &account) override;
      WsvCommandResult setAccountKV(
          const shared_model::interface::types::AccountIdType &account_id,
          const shared_model::interface::types::AccountIdType
              &creator_account_id,
          const std::string &key,
          const std::string &val) override;
      WsvCommandResult insertAsset(
          const shared_model::interface::Asset &asset) override;
      WsvCommandResult upsertAccountAsset(
          const shared_model::interface::AccountAsset &asset) override;
      WsvCommandResult insertSignatory(
          const shared_model::interface::types::PubkeyType &signatory) override;
      WsvCommandResult insertAccountSignatory(
          const shared_model::interface::types::AccountIdType &account_id,
          const shared_model::interface::types::PubkeyType &signatory) override;
      WsvCommandResult deleteAccountSignatory(
          const shared_model::interface::types::AccountIdType &account_id,
          const shared_model::interface::types::PubkeyType &signatory) override;
      WsvCommandResult deleteSignatory(
          const shared_model::interface::types::PubkeyType &signatory) override;
      WsvCommandResult insertPeer(
          const shared_model::interface::Peer &peer) override;
      WsvCommandResult deletePeer(
          const shared_model::interface::Peer &peer) override;
      WsvCommandResult insertDomain(
          const shared_model::interface::Domain &domain) override;
      WsvCommandResult insertAccountGrantablePermission(
          const shared_model::interface::types::AccountIdType
              &permittee_account_id,
          const shared_model::interface::types::AccountIdType &account_id,
          shared_model::interface::permissions::Grantable permission) override;

      WsvCommandResult deleteAccountGrantablePermission(
          const shared_model::interface::types::AccountIdType
              &permittee_account_id,
          const shared_model::interface::types::AccountIdType &account_id,
          shared_model::interface::permissions::Grantable permission) override;

     private:
        MutableWsv &db_;
    };
  }  // namespace newstorage
}  // namespace iroha

#endif  // IROHA_WSV_COMMAND_IMPL_HPP
