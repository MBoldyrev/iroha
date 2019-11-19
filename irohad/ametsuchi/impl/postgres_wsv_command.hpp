/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_POSTGRES_WSV_COMMAND_HPP
#define IROHA_POSTGRES_WSV_COMMAND_HPP

#include "ametsuchi/wsv_command.hpp"

#include "ametsuchi/impl/soci_utils.hpp"

namespace iroha {
  namespace ametsuchi {

    class PostgresWsvCommand : public WsvCommand {
     public:
      explicit PostgresWsvCommand(soci::session &sql);
      WsvCommandResult insertRole(
          const shared_model::types::RoleIdType &role_name) override;

      WsvCommandResult insertAccountRole(
          const shared_model::types::AccountIdType &account_id,
          const shared_model::types::RoleIdType &role_name) override;
      WsvCommandResult deleteAccountRole(
          const shared_model::types::AccountIdType &account_id,
          const shared_model::types::RoleIdType &role_name) override;

      WsvCommandResult insertRolePermissions(
          const shared_model::types::RoleIdType &role_id,
          const shared_model::RolePermissionSet &permissions) override;

      WsvCommandResult insertAccount(
          const shared_model::Account &account) override;
      WsvCommandResult updateAccount(
          const shared_model::Account &account) override;
      WsvCommandResult setAccountKV(
          const shared_model::types::AccountIdType &account_id,
          const shared_model::types::AccountIdType &creator_account_id,
          const std::string &key,
          const std::string &val) override;
      WsvCommandResult insertAsset(const shared_model::Asset &asset) override;
      WsvCommandResult upsertAccountAsset(
          const shared_model::AccountAsset &asset) override;
      WsvCommandResult insertSignatory(
          const shared_model::types::PubkeyType &signatory) override;
      WsvCommandResult insertAccountSignatory(
          const shared_model::types::AccountIdType &account_id,
          const shared_model::types::PubkeyType &signatory) override;
      WsvCommandResult deleteAccountSignatory(
          const shared_model::types::AccountIdType &account_id,
          const shared_model::types::PubkeyType &signatory) override;
      WsvCommandResult deleteSignatory(
          const shared_model::types::PubkeyType &signatory) override;
      WsvCommandResult insertPeer(const shared_model::Peer &peer) override;
      WsvCommandResult deletePeer(const shared_model::Peer &peer) override;
      WsvCommandResult insertDomain(
          const shared_model::Domain &domain) override;
      WsvCommandResult insertAccountGrantablePermission(
          const shared_model::types::AccountIdType &permittee_account_id,
          const shared_model::types::AccountIdType &account_id,
          shared_model::permissions::Grantable permission) override;

      WsvCommandResult deleteAccountGrantablePermission(
          const shared_model::types::AccountIdType &permittee_account_id,
          const shared_model::types::AccountIdType &account_id,
          shared_model::permissions::Grantable permission) override;

     private:
      soci::session &sql_;
    };
  }  // namespace ametsuchi
}  // namespace iroha

#endif  // IROHA_POSTGRES_WSV_COMMAND_HPP
