/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "command_executor_impl.hpp"

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
#include "utils/string_builder.hpp"

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
      const iroha::newstorage::CommandError::ErrorCodeType code,
      std::string &&query_args) noexcept {
    return iroha::expected::makeError(iroha::newstorage::CommandError{
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
  boost::optional<iroha::newstorage::CommandError::ErrorCodeType>
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
  iroha::newstorage::CommandResult getCommandError(
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

  /**
   * Generate an SQL subquery which checks if creator has corresponding
   * permissions for target account
   * It verifies individual, domain, and global permissions, and returns true if
   * any of listed permissions is present
   */
  auto hasQueryPermission(
      const shared_model::interface::types::AccountIdType &creator,
      const shared_model::interface::types::AccountIdType &target_account,
      Role indiv_permission_id,
      Role all_permission_id,
      Role domain_permission_id,
      const shared_model::interface::types::DomainIdType &creator_domain,
      const shared_model::interface::types::DomainIdType
          &target_account_domain) {
    const auto bits = shared_model::interface::RolePermissionSet::size();
    const auto perm_str =
        shared_model::interface::RolePermissionSet({indiv_permission_id})
            .toBitstring();
    const auto all_perm_str =
        shared_model::interface::RolePermissionSet({all_permission_id})
            .toBitstring();
    const auto domain_perm_str =
        shared_model::interface::RolePermissionSet({domain_permission_id})
            .toBitstring();

    boost::format cmd(R"(
    has_indiv_perm AS (
      SELECT (COALESCE(bit_or(rp.permission), '0'::bit(%1%))
      & '%3%') = '%3%' FROM role_has_permissions AS rp
          JOIN account_has_roles AS ar on ar.role_id = rp.role_id
          WHERE ar.account_id = %2%
    ),
    has_all_perm AS (
      SELECT (COALESCE(bit_or(rp.permission), '0'::bit(%1%))
      & '%4%') = '%4%' FROM role_has_permissions AS rp
          JOIN account_has_roles AS ar on ar.role_id = rp.role_id
          WHERE ar.account_id = %2%
    ),
    has_domain_perm AS (
      SELECT (COALESCE(bit_or(rp.permission), '0'::bit(%1%))
      & '%5%') = '%5%' FROM role_has_permissions AS rp
          JOIN account_has_roles AS ar on ar.role_id = rp.role_id
          WHERE ar.account_id = %2%
    ),
    has_query_perm AS (
      SELECT (%2% = %6% AND (SELECT * FROM has_indiv_perm))
          OR (SELECT * FROM has_all_perm)
          OR (%7% = %8% AND (SELECT * FROM has_domain_perm)) AS perm
    )
    )");

    return (cmd % bits % creator % perm_str % all_perm_str % domain_perm_str
            % target_account % creator_domain % target_account_domain)
        .str();
  }

  std::string checkAccountDomainRoleOrGlobalRolePermission(
      Role global_permission,
      Role domain_permission,
      const shared_model::interface::types::AccountIdType &creator_id,
      const shared_model::interface::types::AssetIdType
          &id_with_target_domain) {
    std::string query = (boost::format(R"(WITH
          has_global_role_perm AS (%1%),
          has_domain_role_perm AS (%2%)
          SELECT CASE
                           WHEN (SELECT * FROM has_global_role_perm) THEN true
                           WHEN ((split_part(%3%, '@', 2) = split_part(%4%, '#', 2))) THEN
                               CASE
                                   WHEN (SELECT * FROM has_domain_role_perm) THEN true
                                   ELSE false
                                END
                           ELSE false END
          )") % checkAccountRolePermission(global_permission, creator_id)
                         % checkAccountRolePermission(domain_permission,
                                                      creator_id)
                         % creator_id % id_with_target_domain)
                            .str();
    return query;
  }

  std::string checkAccountHasRoleOrGrantablePerm(
      Role role,
      Grantable grantable,
      const shared_model::interface::types::AccountIdType &creator_id,
      const shared_model::interface::types::AccountIdType &account_id) {
    return (boost::format(R"(WITH
          has_role_perm AS (%s),
          has_grantable_perm AS (%s)
          SELECT CASE
                           WHEN (SELECT * FROM has_grantable_perm) THEN true
                           WHEN (%s = %s) THEN
                               CASE
                                   WHEN (SELECT * FROM has_role_perm) THEN true
                                   ELSE false
                                END
                           ELSE false END
          )")
            % checkAccountRolePermission(role, creator_id)
            % checkAccountGrantablePermission(grantable, creator_id, account_id)
            % creator_id % account_id)
        .str();
  }
}  // namespace

namespace iroha {
  namespace newstorage {

/*
    class CommandExecutorImpl::StatementExecutor {
     public:
      StatementExecutor(
          std::unique_ptr<CommandStatements> &statements,
          bool enable_validation,
          std::string command_name,
          std::shared_ptr<shared_model::interface::PermissionToString>
              perm_converter)
          : statement_(statements->getStatement(enable_validation)),
            command_name_(std::move(command_name)),
            perm_converter_(std::move(perm_converter)) {
        arguments_string_builder_.init(command_name_)
            .append("Validation", std::to_string(enable_validation));
      }

      template <typename T,
                typename = decltype(soci::use(std::declval<T>(),
                                              std::string{}))>
      void use(const std::string &argument_name, const T &value) {
        statement_.exchange(soci::use(value, argument_name));
        addArgumentToString(argument_name, value);
      }

      void use(const std::string &argument_name, const Role &permission) {
        temp_values_.emplace_front(
            shared_model::interface::RolePermissionSet({permission})
                .toBitstring());
        statement_.exchange(soci::use(temp_values_.front(), argument_name));
        addArgumentToString(argument_name,
                            perm_converter_->toString(permission));
      }

      void use(const std::string &argument_name, const Grantable &permission) {
        temp_values_.emplace_front(
            shared_model::interface::GrantablePermissionSet({permission})
                .toBitstring());
        statement_.exchange(soci::use(temp_values_.front(), argument_name));
        addArgumentToString(argument_name,
                            perm_converter_->toString(permission));
      }

      void use(
          const std::string &argument_name,
          const shared_model::interface::RolePermissionSet &permission_set) {
        temp_values_.emplace_front(permission_set.toBitstring());
        statement_.exchange(soci::use(temp_values_.front(), argument_name));
        addArgumentToString(
            argument_name,
            boost::algorithm::join(perm_converter_->toString(permission_set),
                                   ", "));
      }

      void use(const std::string &argument_name, bool value) {
        statement_.exchange(
            soci::use(value ? kPgTrue : kPgFalse, argument_name));
        addArgumentToString(argument_name, std::to_string(value));
      }

      // TODO IR-597 mboldyrev 2019.08.10: build args string on demand
      void addArgumentToString(const std::string &argument_name,
                               const std::string &value) {
        arguments_string_builder_.append(argument_name, value);
      }

      template <typename T>
      std::enable_if_t<std::is_arithmetic<T>::value> addArgumentToString(
          const std::string &argument_name, const T &value) {
        addArgumentToString(argument_name, std::to_string(value));
      }

      iroha::newstorage::CommandResult execute() noexcept {
        try {
          soci::row r;
          statement_.define_and_bind();
          statement_.exchange_for_rowset(soci::into(r));
          statement_.execute();
          auto result = statement_.fetch() ? r.get<int>(0) : 1;
          statement_.bind_clean_up();
          temp_values_.clear();
          if (result != 0) {
            return makeCommandError(
                command_name_, result, arguments_string_builder_.finalize());
          }
          return {};
        } catch (const std::exception &e) {
          statement_.bind_clean_up();
          temp_values_.clear();
          return getCommandError(
              command_name_, e.what(), arguments_string_builder_.finalize());
        }
      }

     private:
      soci::statement &statement_;
      std::string command_name_;
      std::shared_ptr<shared_model::interface::PermissionToString>
          perm_converter_;
      shared_model::detail::PrettyStringBuilder arguments_string_builder_;
      std::forward_list<std::string> temp_values_;
    };

*/

    std::string CommandError::toString() const {
      return (boost::format("%s: %d with extra info '%s'") % command_name
              % error_code % error_extra)
          .str();
    }

    CommandExecutorImpl::CommandExecutorImpl(
        //std::unique_ptr<soci::session> sql,
        std::shared_ptr<shared_model::interface::PermissionToString>
            perm_converter)
    {}/*    : sql_(std::move(sql)), perm_converter_{std::move(perm_converter)} {
      initStatements();
    }*/

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

    //soci::session &CommandExecutorImpl::getSession() {
    //  return *sql_;
    //}

    /*
    CommandResult CommandExecutorImpl::operator()(
        const shared_model::interface::AddAssetQuantity &command,
        const shared_model::interface::types::AccountIdType &creator_account_id,
        bool do_validation) {
      auto &asset_id = command.assetId();
      auto quantity = command.amount().toStringRepr();
      int precision = command.amount().precision();

      StatementExecutor executor(add_asset_quantity_statements_,
                                 do_validation,
                                 "AddAssetQuantity",
                                 perm_converter_);
      executor.use("creator", creator_account_id);
      executor.use("asset_id", asset_id);
      executor.use("precision", precision);
      executor.use("quantity", quantity);

      return executor.execute();
    }
     */

    CommandResult CommandExecutorImpl::operator()(
        const shared_model::interface::AddPeer &command,
        const shared_model::interface::types::AccountIdType &creator_account_id,
        bool do_validation) {
      auto &peer = command.peer();

      StatementExecutor executor(
          add_peer_statements_, do_validation, "AddPeer", perm_converter_);
      executor.use("creator", creator_account_id);
      executor.use("address", peer.address());
      executor.use("pubkey", peer.pubkey().hex());

      return executor.execute();
    }

    CommandResult CommandExecutorImpl::operator()(
        const shared_model::interface::AddSignatory &command,
        const shared_model::interface::types::AccountIdType &creator_account_id,
        bool do_validation) {
      auto &target = command.accountId();
      const auto &pubkey = command.pubkey().hex();

      StatementExecutor executor(add_signatory_statements_,
                                 do_validation,
                                 "AddSignatory",
                                 perm_converter_);
      executor.use("creator", creator_account_id);
      executor.use("target", target);
      executor.use("pubkey", pubkey);

      return executor.execute();
    }

    CommandResult CommandExecutorImpl::operator()(
        const shared_model::interface::AppendRole &command,
        const shared_model::interface::types::AccountIdType &creator_account_id,
        bool do_validation) {
      auto &target = command.accountId();
      auto &role = command.roleName();

      StatementExecutor executor(append_role_statements_,
                                 do_validation,
                                 "AppendRole",
                                 perm_converter_);
      executor.use("creator", creator_account_id);
      executor.use("target", target);
      executor.use("role", role);

      return executor.execute();
    }

    CommandResult CommandExecutorImpl::operator()(
        const shared_model::interface::CompareAndSetAccountDetail &command,
        const shared_model::interface::types::AccountIdType &creator_account_id,
        bool do_validation) {
      std::string new_json_value = makeJsonString(command.value());
      const std::string expected_json_value =
          makeJsonString(command.oldValue().value_or(""));

      StatementExecutor executor(compare_and_set_account_detail_statements_,
                                 do_validation,
                                 "CompareAndSetAccountDetail",
                                 perm_converter_);
      executor.use("creator", creator_account_id);
      executor.use("target", command.accountId());
      executor.use("key", command.key());
      executor.use("new_value", new_json_value);
      executor.use("have_expected_value",
                   static_cast<bool>(command.oldValue()));
      executor.use("expected_value", expected_json_value);
      executor.use("creator_domain", getDomainFromName(creator_account_id));
      executor.use("target_domain", getDomainFromName(command.accountId()));

      return executor.execute();
    }

    CommandResult CommandExecutorImpl::operator()(
        const shared_model::interface::CreateAccount &command,
        const shared_model::interface::types::AccountIdType &creator_account_id,
        bool do_validation) {
      auto &account_name = command.accountName();
      auto &domain_id = command.domainId();
      auto &pubkey = command.pubkey().hex();
      shared_model::interface::types::AccountIdType account_id =
          account_name + "@" + domain_id;

      StatementExecutor executor(create_account_statements_,
                                 do_validation,
                                 "CreateAccount",
                                 perm_converter_);
      executor.use("creator", creator_account_id);
      executor.use("account_id", account_id);
      executor.use("domain", domain_id);
      executor.use("pubkey", pubkey);

      return executor.execute();
    }

    CommandResult CommandExecutorImpl::operator()(
        const shared_model::interface::CreateAsset &command,
        const shared_model::interface::types::AccountIdType &creator_account_id,
        bool do_validation) {
      auto &domain_id = command.domainId();
      auto asset_id = command.assetName() + "#" + domain_id;
      int precision = command.precision();

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
        const shared_model::interface::types::AccountIdType &creator_account_id,
        bool do_validation) {
      auto &domain_id = command.domainId();
      auto &default_role = command.userDefaultRole();

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
        const shared_model::interface::types::AccountIdType &creator_account_id,
        bool do_validation) {
      auto &role_id = command.roleName();
      auto &permissions = command.rolePermissions();
      auto perm_str = permissions.toBitstring();

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
        const shared_model::interface::types::AccountIdType &creator_account_id,
        bool do_validation) {
      auto &account_id = command.accountId();
      auto &role_name = command.roleName();

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
        const shared_model::interface::types::AccountIdType &creator_account_id,
        bool do_validation) {
      auto &permittee_account_id = command.accountId();
      auto granted_perm = command.permissionName();
      auto required_perm =
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
        const shared_model::interface::types::AccountIdType &creator_account_id,
        bool do_validation) {
      auto pubkey = command.pubkey().hex();

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
        const shared_model::interface::types::AccountIdType &creator_account_id,
        bool do_validation) {
      auto &account_id = command.accountId();
      auto &pubkey = command.pubkey().hex();

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
        const shared_model::interface::types::AccountIdType &creator_account_id,
        bool do_validation) {
      auto &permittee_account_id = command.accountId();
      auto revoked_perm = command.permissionName();

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
        const shared_model::interface::types::AccountIdType &creator_account_id,
        bool do_validation) {
      auto &account_id = command.accountId();
      auto &key = command.key();
      auto &value = command.value();
      std::string json_value = makeJsonString(value);

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
        const shared_model::interface::types::AccountIdType &creator_account_id,
        bool do_validation) {
      auto &account_id = command.accountId();
      int quorum = command.newQuorum();

      StatementExecutor executor(
          set_quorum_statements_, do_validation, "SetQuorum", perm_converter_);
      executor.use("creator", creator_account_id);
      executor.use("target", account_id);
      executor.use("quorum", quorum);

      return executor.execute();
    }

    CommandResult CommandExecutorImpl::operator()(
        const shared_model::interface::SubtractAssetQuantity &command,
        const shared_model::interface::types::AccountIdType &creator_account_id,
        bool do_validation) {
      auto &asset_id = command.assetId();
      auto quantity = command.amount().toStringRepr();
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
        const shared_model::interface::types::AccountIdType &creator_account_id,
        bool do_validation) {
      auto &src_account_id = command.srcAccountId();
      auto &dest_account_id = command.destAccountId();
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

  }  // namespace newstorage
}  // namespace iroha
