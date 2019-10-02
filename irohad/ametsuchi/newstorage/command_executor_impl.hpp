/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_COMMAND_EXECUTOR_IMPL_HPP
#define IROHA_COMMAND_EXECUTOR_IMPL_HPP

#include "ametsuchi/command_executor.hpp"
#include "utils/string_builder.hpp"

namespace shared_model {
  namespace interface {
    class AddAssetQuantity;
    class AddPeer;
    class AddSignatory;
    class AppendRole;
    class CompareAndSetAccountDetail;
    class CreateAccount;
    class CreateAsset;
    class CreateDomain;
    class CreateRole;
    class DetachRole;
    class GrantPermission;
    class PermissionToString;
    class RemovePeer;
    class RemoveSignatory;
    class RevokePermission;
    class SetAccountDetail;
    class SetQuorum;
    class SubtractAssetQuantity;
    class TransferAsset;
  }  // namespace interface
}  // namespace shared_model

namespace iroha {
  namespace newstorage {

    class MutableWsv;

    class CommandExecutorImpl final : public ametsuchi::CommandExecutor {
     public:
      using CommandResult = iroha::ametsuchi::CommandResult;

      CommandExecutorImpl(
          MutableWsv &db_,
          std::shared_ptr<shared_model::interface::PermissionToString>
              perm_converter);

      ~CommandExecutorImpl();

      CommandResult execute(const shared_model::interface::Command &cmd,
                            const shared_model::interface::types::AccountIdType
                                &creator_account_id,
                            bool do_validation) override;

      //??? &getSession();

      CommandResult operator()(
          const shared_model::interface::AddAssetQuantity &command,
          const shared_model::interface::types::AccountIdType
              &creator_account_id,
          bool do_validation);

      CommandResult operator()(
          const shared_model::interface::AddPeer &command,
          const shared_model::interface::types::AccountIdType
              &creator_account_id,
          bool do_validation);

      CommandResult operator()(
          const shared_model::interface::AddSignatory &command,
          const shared_model::interface::types::AccountIdType
              &creator_account_id,
          bool do_validation);

      CommandResult operator()(
          const shared_model::interface::AppendRole &command,
          const shared_model::interface::types::AccountIdType
              &creator_account_id,
          bool do_validation);

      CommandResult operator()(
          const shared_model::interface::CompareAndSetAccountDetail &command,
          const shared_model::interface::types::AccountIdType
              &creator_account_id,
          bool do_validation);

      CommandResult operator()(
          const shared_model::interface::CreateAccount &command,
          const shared_model::interface::types::AccountIdType
              &creator_account_id,
          bool do_validation);

      CommandResult operator()(
          const shared_model::interface::CreateAsset &command,
          const shared_model::interface::types::AccountIdType
              &creator_account_id,
          bool do_validation);

      CommandResult operator()(
          const shared_model::interface::CreateDomain &command,
          const shared_model::interface::types::AccountIdType
              &creator_account_id,
          bool do_validation);

      CommandResult operator()(
          const shared_model::interface::CreateRole &command,
          const shared_model::interface::types::AccountIdType
              &creator_account_id,
          bool do_validation);

      CommandResult operator()(
          const shared_model::interface::DetachRole &command,
          const shared_model::interface::types::AccountIdType
              &creator_account_id,
          bool do_validation);

      CommandResult operator()(
          const shared_model::interface::GrantPermission &command,
          const shared_model::interface::types::AccountIdType
              &creator_account_id,
          bool do_validation);

      CommandResult operator()(
          const shared_model::interface::RemovePeer &command,
          const shared_model::interface::types::AccountIdType
              &creator_account_id,
          bool do_validation);

      CommandResult operator()(
          const shared_model::interface::RemoveSignatory &command,
          const shared_model::interface::types::AccountIdType
              &creator_account_id,
          bool do_validation);

      CommandResult operator()(
          const shared_model::interface::RevokePermission &command,
          const shared_model::interface::types::AccountIdType
              &creator_account_id,
          bool do_validation);

      CommandResult operator()(
          const shared_model::interface::SetAccountDetail &command,
          const shared_model::interface::types::AccountIdType
              &creator_account_id,
          bool do_validation);

      CommandResult operator()(
          const shared_model::interface::SetQuorum &command,
          const shared_model::interface::types::AccountIdType
              &creator_account_id,
          bool do_validation);

      CommandResult operator()(
          const shared_model::interface::SubtractAssetQuantity &command,
          const shared_model::interface::types::AccountIdType
              &creator_account_id,
          bool do_validation);

      CommandResult operator()(
          const shared_model::interface::TransferAsset &command,
          const shared_model::interface::types::AccountIdType
              &creator_account_id,
          bool do_validation);

     private:
      MutableWsv &db_;
      std::shared_ptr<shared_model::interface::PermissionToString> perm_converter_;
      shared_model::detail::PrettyStringBuilder string_builder_;
    };
  }  // namespace newstorage
}  // namespace iroha

#endif  // IROHA_COMMAND_EXECUTOR_IMPL_HPP
