/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ametsuchi/newstorage/command_executor_impl.hpp"
#include "ametsuchi/newstorage/mutable_wsv.hpp"

#include <forward_list>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/format.hpp>
#include "cryptography/public_key.hpp"
#include "interfaces/commands/add_asset_quantity.hpp"
#include "interfaces/commands/add_peer.hpp"
#include "interfaces/commands/add_signatory.hpp"
#include "interfaces/commands/append_role.hpp"
#include "interfaces/commands/command.hpp"
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
#include "interfaces/commands/subtract_asset_quantity.hpp"
#include "interfaces/commands/transfer_asset.hpp"
#include "interfaces/common_objects/types.hpp"
#include "interfaces/permission_to_string.hpp"

using shared_model::interface::permissions::Grantable;
using shared_model::interface::permissions::Role;

namespace {
  constexpr size_t kRolePermissionSetSize =
      shared_model::interface::RolePermissionSet::size();
  constexpr size_t kGrantablePermissionSetSize =
      shared_model::interface::GrantablePermissionSet::size();

  std::string makeJsonString(std::string value) {
    return std::string{"\""} + value + "\"";
  }

  iroha::expected::Error<iroha::ametsuchi::CommandError> makeCommandError(
      std::string command_name,
      const iroha::ametsuchi::CommandError::ErrorCodeType code,
      std::string &&query_args) noexcept {
    return iroha::expected::makeError(iroha::ametsuchi::CommandError{
        std::move(command_name), code, std::move(query_args)});
  }

  /// mapping between pairs of SQL error substrings and related fake error
  /// codes, which are indices in this collection
  const std::vector<std::tuple<std::string, std::string>> kSqlToFakeErrorCode =
      {std::make_tuple("Key (account_id)=", "is not present in table"),
       std::make_tuple("Key (permittee_account_id)", "is not present in table"),
       std::make_tuple("Key (role_id)=", "is not present in table"),
       std::make_tuple("Key (domain_id)=", "is not present in table"),
       std::make_tuple("Key (asset_id)=", "already exists"),
       std::make_tuple("Key (domain_id)=", "already exists"),
       std::make_tuple("Key (role_id)=", "already exists"),
       std::make_tuple("Key (account_id, public_key)=", "already exists"),
       std::make_tuple("Key (account_id)=", "already exists"),
       std::make_tuple("Key (default_role)=", "is not present in table")};

  /// mapping between command name, fake error code and related real error code
  const std::map<std::string, std::map<int, int>> kCmdNameToErrorCode{
      std::make_pair(
          "AddSignatory",
          std::map<int, int>{std::make_pair(0, 3), std::make_pair(7, 4)}),
      std::make_pair(
          "AppendRole",
          std::map<int, int>{std::make_pair(0, 3), std::make_pair(2, 4)}),
      std::make_pair(
          "DetachRole",
          std::map<int, int>{std::make_pair(0, 3), std::make_pair(2, 5)}),
      std::make_pair("RemoveSignatory",
                     std::map<int, int>{std::make_pair(0, 3)}),
      std::make_pair("SetAccountDetail",
                     std::map<int, int>{std::make_pair(0, 3)}),
      std::make_pair("SetQuorum", std::map<int, int>{std::make_pair(0, 3)}),
      std::make_pair("GrantPermission",
                     std::map<int, int>{std::make_pair(1, 3)}),
      std::make_pair("RevokePermission",
                     std::map<int, int>{std::make_pair(1, 3)}),
      std::make_pair(
          "CreateAccount",
          std::map<int, int>{std::make_pair(3, 3), std::make_pair(8, 4)}),
      std::make_pair(
          "CreateAsset",
          std::map<int, int>{std::make_pair(3, 3), std::make_pair(4, 4)}),
      std::make_pair(
          "CreateDomain",
          std::map<int, int>{std::make_pair(5, 3), std::make_pair(9, 4)}),
      std::make_pair("CreateRole", std::map<int, int>{std::make_pair(6, 3)}),
      std::make_pair("AddSignatory", std::map<int, int>{std::make_pair(7, 4)})};

  /**
   * Get a real error code based on the fake one and a command name
   * @param fake_error_code - inner error code to be translated into the user's
   * one
   * @param command_name of the failed command
   * @return real error code
   */
  boost::optional<iroha::ametsuchi::CommandError::ErrorCodeType>
  getRealErrorCode(size_t fake_error_code, const std::string &command_name) {
    auto fake_to_real_code = kCmdNameToErrorCode.find(command_name);
    if (fake_to_real_code == kCmdNameToErrorCode.end()) {
      return {};
    }

    auto real_code = fake_to_real_code->second.find(fake_error_code);
    if (real_code == fake_to_real_code->second.end()) {
      return {};
    }

    return real_code->second;
  }

  // TODO [IR-1830] Akvinikym 31.10.18: make benchmarks to compare exception
  // parsing vs nested queries
  /**
   * Get an error code from the text SQL error
   * @param command_name - name of the failed command
   * @param error - string error, which SQL gave out
   * @param query_args - a string representation of query arguments
   * @return command_error structure
   */
  iroha::ametsuchi::CommandResult getCommandError(
      std::string command_name,
      const std::string &error,
      std::string &&query_args) noexcept {
    std::string key, to_be_presented;
    bool errors_matched;

    // go through mapping of SQL errors and get index of the current error - it
    // is "fake" error code
    for (size_t fakeErrorCode = 0; fakeErrorCode < kSqlToFakeErrorCode.size();
         ++fakeErrorCode) {
      std::tie(key, to_be_presented) = kSqlToFakeErrorCode[fakeErrorCode];
      errors_matched = error.find(key) != std::string::npos
          and error.find(to_be_presented) != std::string::npos;
      if (errors_matched) {
        if (auto real_error_code =
                getRealErrorCode(fakeErrorCode, command_name)) {
          return makeCommandError(
              std::move(command_name), *real_error_code, std::move(query_args));
        }
        break;
      }
    }
    // parsing is not successful, return the general error
    return makeCommandError(std::move(command_name), 1, std::move(query_args));
  }

  shared_model::interface::types::DomainIdType getDomainFromName(
      const shared_model::interface::types::AccountIdType &account_id) {
    // TODO 03.10.18 andrei: IR-1728 Move getDomainFromName to shared_model
    std::vector<std::string> res;
    boost::split(res, account_id, boost::is_any_of("@"));
    return res.at(1);
  }

}  // namespace

namespace iroha {
  namespace ametsuchi {
    std::string CommandError::toString() const {
      return (boost::format("%s: %d with extra info '%s'") % command_name
              % error_code % error_extra)
          .str();
    }
  }  // namespace ametsuchi

  namespace newstorage {

    using ametsuchi::CommandError;
    using ametsuchi::CommandResult;

    CommandError::ErrorCodeType fromResultCode(ResultCode res) {
      //      switch (res) {
      // case TODO
      //     }
      return 1;  // general error code for command responses
    }

    namespace {

      struct ErrorBuilder {
        using SB = shared_model::detail::PrettyStringBuilder;
        SB &sb;
        CommandError err;

        ErrorBuilder(SB &string_builder,
                     const char *command_name,
                     ResultCode res,
                     std::string error_str,
                     bool validation)
            : sb(string_builder) {
          err.command_name = std::move(command_name);
          err.error_code = fromResultCode(res);
          sb.init(command_name);
          sb.append("Error", std::move(error_str));
          sb.append("Validation", validation ? "1" : "0");
        }

        ErrorBuilder &append(const char *arg_name, const std::string& value) {
          sb.append(arg_name, value);
          return *this;
        }

        template <typename Arg>
        ErrorBuilder &append(const char *arg_name, const Arg &value) {
          return append(arg_name, std::to_string(value));
        }

        CommandResult finalize() {
          err.error_extra = sb.finalize();
          return iroha::expected::makeError(std::move(err));
        }
      };
    }  // namespace

    CommandExecutorImpl::CommandExecutorImpl(
        MutableWsv &db,
        std::shared_ptr<shared_model::interface::PermissionToString>
            perm_converter)
        : db_(db), perm_converter_{std::move(perm_converter)} {}

    CommandExecutorImpl::~CommandExecutorImpl() = default;

    CommandResult CommandExecutorImpl::execute(
        const shared_model::interface::Command &cmd,
        const shared_model::interface::types::AccountIdType &creator_account_id,
        bool do_validation) {
      return boost::apply_visitor(
          [this, &creator_account_id, do_validation](const auto &command) {
            return (*this)(command, creator_account_id, do_validation);
          },
          cmd.get());
    }

    CommandResult CommandExecutorImpl::operator()(
        const shared_model::interface::AddAssetQuantity &command,
        const shared_model::interface::types::AccountIdType &creator_account_id,
        bool do_validation) {
      // TODO handle parse error or rely on stateless validation?
      uint256_t quantity(command.amount().toStringRepr());

      ResultCode res = db_.addAssetQuantity(creator_account_id,
                                            do_validation,
                                            command.assetId(),
                                            quantity,
                                            command.amount().precision());

      command.toString();
      if (res != ResultCode::kOk) {
        return ErrorBuilder(string_builder_,
                            "AddAssetQuantity",
                            res,
                            db_.getLastError(),
                            do_validation)
            .append("creator", creator_account_id)
            .append("asset_id", command.assetId())
            .append("precision", command.amount().precision())
            .append("quantity", command.amount().toStringRepr())
            .finalize();
      }

      return {};
    }

    CommandResult CommandExecutorImpl::operator()(
        const shared_model::interface::AddPeer &command,
        const shared_model::interface::types::AccountIdType &creator_account_id,
        bool do_validation) {
      auto &peer = command.peer();

      ResultCode res = db_.addPeer(creator_account_id,
                                   do_validation,
                                   peer.pubkey().hex(),
                                   peer.address());
      if (res != ResultCode::kOk) {
        return ErrorBuilder(string_builder_,
                            "AddPeer",
                            res,
                            db_.getLastError(),
                            do_validation)
            .append("creator", creator_account_id)
            .append("address", peer.address())
            .append("pubkey", peer.pubkey().hex())
            .finalize();
      }

      return {};
    }

    CommandResult CommandExecutorImpl::operator()(
        const shared_model::interface::AddSignatory &command,
        const shared_model::interface::types::AccountIdType &creator_account_id,
        bool do_validation) {
      ResultCode res = db_.addSignatory(creator_account_id,
                                        do_validation,
                                        command.accountId(),
                                        command.pubkey().hex());
      if (res != ResultCode::kOk) {
        return ErrorBuilder(string_builder_,
                            "AddSignatory",
                            res,
                            db_.getLastError(),
                            do_validation)
            .append("creator", creator_account_id)
            .append("target", command.accountId())
            .append("pubkey", command.pubkey().hex())
            .finalize();
      }

      return {};
    }

    CommandResult CommandExecutorImpl::operator()(
        const shared_model::interface::AppendRole &command,
        const shared_model::interface::types::AccountIdType &creator_account_id,
        bool do_validation) {
      ResultCode res = db_.appendRole(creator_account_id,
                                      do_validation,
                                      command.accountId(),
                                      command.roleName());

      if (res != ResultCode::kOk) {
        return ErrorBuilder(string_builder_,
                            "AppendRole",
                            res,
                            db_.getLastError(),
                            do_validation)
            .append("creator", creator_account_id)
            .append("target", command.accountId())
            .append("role", command.roleName())
            .finalize();
      }

      return {};
    }

    CommandResult CommandExecutorImpl::operator()(
        const shared_model::interface::CompareAndSetAccountDetail &command,
        const shared_model::interface::types::AccountIdType &creator_account_id,
        bool do_validation) {
      const std::string expected_value = command.oldValue().value_or("");

      ResultCode res =
          db_.compareAndSetAccountDetail(creator_account_id,
                                         do_validation,
                                         command.accountId(),
                                         command.key(),
                                         command.oldValue().value_or(""),
                                         command.value());

      if (res != ResultCode::kOk) {
        return ErrorBuilder(string_builder_,
                            "CompareAndSetAccountDetail",
                            res,
                            db_.getLastError(),
                            do_validation)
            .append("creator", creator_account_id)
            .append("target", command.accountId())
            .append("key", command.key())
            .append("new_value", command.value())
            .append("have_expected_value",
                    static_cast<bool>(command.oldValue()))
            .append("expected_value", expected_value)
            .append("creator_domain", getDomainFromName(creator_account_id))
            .append("target_domain", getDomainFromName(command.accountId()))
            .finalize();
      }

      return {};
    }

    CommandResult CommandExecutorImpl::operator()(
        const shared_model::interface::CreateAccount &command,
        const shared_model::interface::types::AccountIdType &creator_account_id,
        bool do_validation) {
      shared_model::interface::types::AccountIdType account_id =
          command.accountName() + "@" + command.domainId();

      ResultCode res = db_.createAccount(creator_account_id,
                                         do_validation,
                                         account_id,
                                         command.domainId(),
                                         command.pubkey().hex());

      if (res != ResultCode::kOk) {
        return ErrorBuilder(string_builder_,
                            "CreateAccount",
                            res,
                            db_.getLastError(),
                            do_validation)
            .append("creator", creator_account_id)
            .append("account_id", account_id)
            .append("domain", command.domainId())
            .append("pubkey", command.pubkey().hex())
            .finalize();
      }

      return {};
    }
    /*
        CommandResult CommandExecutorImpl::operator()(
            const shared_model::interface::CreateAsset &command,
            const shared_model::interface::types::AccountIdType
       &creator_account_id, bool do_validation) { auto &domain_id =
       command.domainId(); auto asset_id = command.assetName() + "#" +
       domain_id; int precision = command.precision();

          StatementExecutor executor(create_asset_statements_,
                                     do_validation,
                                     "CreateAsset",
                                     perm_converter_);
          executor.use("creator", creator_account_id);
          executor.use("asset_id", asset_id);
          executor.use("domain", domain_id);
          executor.use("precision", precision);

          return executor.execute();
        }

        CommandResult CommandExecutorImpl::operator()(
            const shared_model::interface::CreateDomain &command,
            const shared_model::interface::types::AccountIdType
       &creator_account_id, bool do_validation) { auto &domain_id =
       command.domainId(); auto &default_role = command.userDefaultRole();

          StatementExecutor executor(create_domain_statements_,
                                     do_validation,
                                     "CreateDomain",
                                     perm_converter_);
          executor.use("creator", creator_account_id);
          executor.use("domain", domain_id);
          executor.use("default_role", default_role);

          return executor.execute();
        }

        CommandResult CommandExecutorImpl::operator()(
            const shared_model::interface::CreateRole &command,
            const shared_model::interface::types::AccountIdType
       &creator_account_id, bool do_validation) { auto &role_id =
       command.roleName(); auto &permissions = command.rolePermissions(); auto
       perm_str = permissions.toBitstring();

          StatementExecutor executor(create_role_statements_,
                                     do_validation,
                                     "CreateRole",
                                     perm_converter_);
          executor.use("creator", creator_account_id);
          executor.use("role", role_id);
          executor.use("perms", perm_str);

          return executor.execute();
        }

        CommandResult CommandExecutorImpl::operator()(
            const shared_model::interface::DetachRole &command,
            const shared_model::interface::types::AccountIdType
       &creator_account_id, bool do_validation) { auto &account_id =
       command.accountId(); auto &role_name = command.roleName();

          StatementExecutor executor(detach_role_statements_,
                                     do_validation,
                                     "DetachRole",
                                     perm_converter_);
          executor.use("creator", creator_account_id);
          executor.use("target", account_id);
          executor.use("role", role_name);

          return executor.execute();
        }

        CommandResult CommandExecutorImpl::operator()(
            const shared_model::interface::GrantPermission &command,
            const shared_model::interface::types::AccountIdType
       &creator_account_id, bool do_validation) { auto &permittee_account_id =
       command.accountId(); auto granted_perm = command.permissionName(); auto
       required_perm =
              shared_model::interface::permissions::permissionFor(granted_perm);

          StatementExecutor executor(grant_permission_statements_,
                                     do_validation,
                                     "GrantPermission",
                                     perm_converter_);
          executor.use("creator", creator_account_id);
          executor.use("target", permittee_account_id);
          executor.use("granted_perm", granted_perm);
          executor.use("required_perm", required_perm);

          return executor.execute();
        }

        CommandResult CommandExecutorImpl::operator()(
            const shared_model::interface::RemovePeer &command,
            const shared_model::interface::types::AccountIdType
       &creator_account_id, bool do_validation) { auto pubkey =
       command.pubkey().hex();

          StatementExecutor executor(remove_peer_statements_,
                                     do_validation,
                                     "RemovePeer",
                                     perm_converter_);
          executor.use("creator", creator_account_id);
          executor.use("pubkey", pubkey);

          return executor.execute();
        }

        CommandResult CommandExecutorImpl::operator()(
            const shared_model::interface::RemoveSignatory &command,
            const shared_model::interface::types::AccountIdType
       &creator_account_id, bool do_validation) { auto &account_id =
       command.accountId(); auto &pubkey = command.pubkey().hex();

          StatementExecutor executor(remove_signatory_statements_,
                                     do_validation,
                                     "RemoveSignatory",
                                     perm_converter_);
          executor.use("creator", creator_account_id);
          executor.use("target", account_id);
          executor.use("pubkey", pubkey);

          return executor.execute();
        }

        CommandResult CommandExecutorImpl::operator()(
            const shared_model::interface::RevokePermission &command,
            const shared_model::interface::types::AccountIdType
       &creator_account_id, bool do_validation) { auto &permittee_account_id =
       command.accountId(); auto revoked_perm = command.permissionName();

          StatementExecutor executor(revoke_permission_statements_,
                                     do_validation,
                                     "RevokePermission",
                                     perm_converter_);
          executor.use("creator", creator_account_id);
          executor.use("target", permittee_account_id);
          executor.use("revoked_perm", revoked_perm);

          return executor.execute();
        }

        CommandResult CommandExecutorImpl::operator()(
            const shared_model::interface::SetAccountDetail &command,
            const shared_model::interface::types::AccountIdType
       &creator_account_id, bool do_validation) { auto &account_id =
       command.accountId(); auto &key = command.key(); auto &value =
       command.value(); std::string json_value = makeJsonString(value);

          StatementExecutor executor(set_account_detail_statements_,
                                     do_validation,
                                     "SetAccountDetail",
                                     perm_converter_);
          if (not creator_account_id.empty()) {
            executor.use("creator", creator_account_id);
          } else {
            // When creator is not known, it is genesis block
            static const std::string genesis_creator_account_id = "genesis";
            executor.use("creator", genesis_creator_account_id);
          }
          executor.use("target", account_id);
          executor.use("key", key);
          executor.use("value", json_value);

          return executor.execute();
        }

        CommandResult CommandExecutorImpl::operator()(
            const shared_model::interface::SetQuorum &command,
            const shared_model::interface::types::AccountIdType
       &creator_account_id, bool do_validation) { auto &account_id =
       command.accountId(); int quorum = command.newQuorum();

          StatementExecutor executor(
              set_quorum_statements_, do_validation, "SetQuorum",
       perm_converter_); executor.use("creator", creator_account_id);
          executor.use("target", account_id);
          executor.use("quorum", quorum);

          return executor.execute();
        }

        CommandResult CommandExecutorImpl::operator()(
            const shared_model::interface::SubtractAssetQuantity &command,
            const shared_model::interface::types::AccountIdType
       &creator_account_id, bool do_validation) { auto &asset_id =
       command.assetId(); auto quantity = command.amount().toStringRepr();
          uint32_t precision = command.amount().precision();

          StatementExecutor executor(subtract_asset_quantity_statements_,
                                     do_validation,
                                     "SubtractAssetQuantity",
                                     perm_converter_);
          executor.use("creator", creator_account_id);
          executor.use("asset_id", asset_id);
          executor.use("quantity", quantity);
          executor.use("precision", precision);

          return executor.execute();
        }

        CommandResult CommandExecutorImpl::operator()(
            const shared_model::interface::TransferAsset &command,
            const shared_model::interface::types::AccountIdType
       &creator_account_id, bool do_validation) { auto &src_account_id =
       command.srcAccountId(); auto &dest_account_id = command.destAccountId();
          auto &asset_id = command.assetId();
          auto quantity = command.amount().toStringRepr();
          uint32_t precision = command.amount().precision();

          StatementExecutor executor(transfer_asset_statements_,
                                     do_validation,
                                     "TransferAsset",
                                     perm_converter_);
          executor.use("creator", creator_account_id);
          executor.use("source_account_id", src_account_id);
          executor.use("dest_account_id", dest_account_id);
          executor.use("asset_id", asset_id);
          executor.use("quantity", quantity);
          executor.use("precision", precision);

          return executor.execute();
        }
    */
  }  // namespace newstorage
}  // namespace iroha
