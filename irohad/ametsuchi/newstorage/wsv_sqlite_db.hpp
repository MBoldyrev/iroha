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

      bool peerExists(const std::string& pk);

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

      bool loadAccountPermissions(const std::string& account_id, std::string& perm_string);

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

      // modifying requests throw on errors, return rows affected

      int updateAccountAsset(
          const std::string &account_id, const std::string &asset_id,
          const std::string& balance, uint8_t precision
      );

      int addPeer(const std::string &public_key, const std::string &address);

      int removePeer(const std::string &public_key);

      int dropPeers();

      int addAccountSignatory(
          const std::string &account_id, const std::string &public_key
      );

      int removeAccountSignatory(
          const std::string &account_id, const std::string &public_key
      );

      int createRole(const std::string &role_id, const std::string &permissions);

      int createAccount(
          const std::string &account_id, const std::string &domain_id,
          uint16_t quorum
      );

      int createAsset(
          const std::string &asset_id,
          const std::string& domain_id, uint8_t precision
      );

      int createDomain(const std::string &domain_id, const std::string &role_id);

      int attachAccountRole(const std::string &account_id, const std::string &role_id);

      int detachAccountRole(const std::string &account_id, const std::string &role_id);

      int updateGrantablePermissions(const std::string& from, const std::string& to,
          const std::string& permissions);

      int setQuorum(const std::string& account_id, uint16_t quorum);

      int updateAccountPermissions(const std::string& account_id,
                                      const std::string& permissions);
     private:
      void createSchema();

      std::shared_ptr<SqliteWrapper> db_;
      StatementHandle get_ledger_state_ = 0;
      StatementHandle load_account_assets_ = 0;
      StatementHandle load_grantable_permissions_ = 0;
      StatementHandle load_account_ = 0;
      StatementHandle load_account_permissions_ = 0;
      StatementHandle load_account_signatories_ = 0;
      StatementHandle load_account_roles_ = 0;
      StatementHandle peer_exists_ = 0;
      StatementHandle add_peer_ = 0;
      StatementHandle remove_peer_ = 0;
      StatementHandle add_account_signatory_ = 0;
      StatementHandle remove_account_signatory_ = 0;
      StatementHandle create_role_ = 0;
      StatementHandle create_account_ = 0;
      StatementHandle create_asset_ = 0;
      StatementHandle update_account_asset_ = 0;
      StatementHandle create_domain_ = 0;
      StatementHandle attach_account_role_ = 0;
      StatementHandle detach_account_role_ = 0;
      StatementHandle update_grantable_permissions_ = 0;
      StatementHandle set_quorum_ = 0;
      StatementHandle update_permissions_ = 0;
      std::vector<char> buffer_;
      logger::LoggerPtr log_;
    };
  }  // namespace newstorage
}  // namespace iroha

#endif  // IROHA_WSV_SQLITE_DB_HPP
