/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_WSV_SQLITE_DB_HPP
#define IROHA_WSV_SQLITE_DB_HPP

#include <functional>
#include <string>
#include <vector>
#include "logger/logger_fwd.hpp"

namespace iroha {
  namespace newstorage {
    class SqliteWrapper;

    using Blob = std::vector<char>;
    using StatementHandle = size_t;

    class WsvSqliteDB {
     public:
      WsvSqliteDB(std::shared_ptr<SqliteWrapper> db, logger::LoggerPtr log);

      bool getLedgerState(uint64_t& height, std::string& hash);

      void loadRoles(
          const std::function<
              void(const std::string& role, const std::string& permissions)>&
          callback
      );

      void loadDomains(
          const std::function<
              void(const std::string& domain, const std::string& def_role)>&
          callback
      );

      void loadSignatories(
           const std::function<void(const std::string& signatory, size_t count)>& callback
      );

      void loadPeers(
          const std::function<void(const std::string& pk, const std::string& address)>& callback
      );

      void loadAssets(
          const std::function<
              void(const std::string& id, const std::string& domain, uint8_t precision)>&
          callback
      );

      bool loadAccountAssets(
          const std::string& account_id,
          const std::function<
              void(const std::string& asset_id, const std::string& balance, uint8_t precision)>&
          callback
      );

      bool loadGrantablePermissions(
          const std::string& from, const std::string& to, std::string& perm_string
      );

      bool loadAccount(
          const std::string& account_id, std::string& domain_id, uint16_t& quorum
      );

      bool loadAccountSignatories(
          const std::string &account_id,
          const std::function<void(const std::string &)>& callback
      );

      bool loadAccountRoles(
          const std::string &account_id,
          const std::function<void(const std::string &)>& callback
      );

      // modifying requests throw on errors

      void updateAccountAsset(
          const std::string &account_id, const std::string &asset_id,
          const std::string& balance, uint8_t precision
      );

      void addPeer(const std::string &private_key, const std::string &address);

      void removePeer(const std::string &private_key);

      void dropPeers();

      void updateSignatory(const std::string &private_key, size_t count);

      void removeSignatory(const std::string &private_key);

      void addAccountSignatory(
          const std::string &account_id, const std::string &private_key
      );

      void removeAccountSignatory(
          const std::string &account_id, const std::string &private_key
      );

      void appendRole(const std::string &role_id, const std::string &permissions);

      void createAccount(
          const std::string &account_id, const std::string &domain_id,
          uint16_t quorum
      );

      void createAsset(
          const std::string &asset_id,
          const std::string& domain_id, uint8_t precision
      );

      void createDomain(const std::string &domain_id, const std::string &role_id);

      void attachAccountRole(const std::string &account_id, const std::string &role_id);

      void detachAccountRole(const std::string &account_id, const std::string &role_id);

      void updateGrantablePermissions(const std::string& from, const std::string& to,
          const std::string& permissions);

      void setQuorum(const std::string& account_id, uint16_t quorum);


     private:
      void createSchema();

      std::shared_ptr<SqliteWrapper> db_;
      StatementHandle get_ledger_state_;
      StatementHandle load_account_assets_;
      StatementHandle load_grantable_permissions_;
      StatementHandle load_account_;
      StatementHandle load_account_signatories_;
      StatementHandle load_account_roles_;
      StatementHandle update_account_asset_;
      StatementHandle add_peer_;
      StatementHandle remove_peer_;
      StatementHandle update_signatory_;
      StatementHandle remove_signatory_;
      StatementHandle add_account_signatory_;
      StatementHandle remove_account_signatory_;
      StatementHandle append_role_;
      StatementHandle create_account_;
      StatementHandle create_asset_;
      StatementHandle create_domain_;
      StatementHandle attach_account_role_;
      StatementHandle detach_account_role_;
      StatementHandle update_grantable_permissions_;
      StatementHandle set_quorum_;
      std::vector<char> buffer_;
      logger::LoggerPtr log_;
    };
  }  // namespace newstorage
}  // namespace iroha

#endif  // IROHA_WSV_SQLITE_DB_HPP
