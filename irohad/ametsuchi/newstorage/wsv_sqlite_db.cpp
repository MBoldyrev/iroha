/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ametsuchi/newstorage/wsv_sqlite_db.hpp"
#include "ametsuchi/newstorage/sqlite_wrapper.hpp"
//#include <boost/filesystem/operations.hpp>
#include "sqlite_modern_cpp.h"
//#include "logger/logger.hpp"

namespace iroha {
  namespace newstorage {

    WsvSqliteDB::WsvSqliteDB(std::shared_ptr<SqliteWrapper> db,
                               logger::LoggerPtr log)
        : db_(std::move(db)),
          log_(std::move(log))
    {
      createSchema();
      // TODO get ledger state and log it
    }

    bool WsvSqliteDB::getLedgerState(uint64_t &height, std::string &hash) {
      db_->getStatement(get_ledger_state_) >> std::tie(height, hash);
      return (height > 0);
    }

    void WsvSqliteDB::loadRoles(
        const std::function<void(const std::string& role, const std::string& permissions)>&
        callback)
    {
      *db_ << "SELECT * from role" >> callback;
    }

    void WsvSqliteDB::loadDomains(
        const std::function<
            void(const std::string& role, const std::string& domain)>&
        callback
    ) {
      *db_ << "SELECT * from domain" >> callback;
    }

    void WsvSqliteDB::loadSignatories(
        const std::function<void(const std::string& signatory, size_t count)>& callback
    ) {
      *db_ << "SELECT * from signatory" >> callback;
    }

    void WsvSqliteDB::loadPeers(
        const std::function<void(const std::string& pk, const std::string& address)>& callback
    ) {
      *db_ << "SELECT * from peer" >> callback;
    }

    void WsvSqliteDB::loadAssets(
        const std::function<
            void(const std::string& id, const std::string& domain, uint8_t precision)>&
        callback
    ) {
      *db_ << "SELECT * from asset" >> callback;
    }

    bool WsvSqliteDB::loadAccountAssets(
        const std::string& account_id,
        const std::function<
            void(const std::string& asset_id, const std::string& balance, uint8_t precision)>&
        callback
    ) {
      try {
        db_->getStatement(load_account_assets_) << account_id
          >> callback;
        return true;
      } catch (const std::exception& e) {
        // TODO log
      }
      return false;
    }

    bool WsvSqliteDB::loadGrantablePermissions(
        const std::string& from, const std::string& to, std::string& perm_string
    ) {
      try {
        db_->getStatement(load_grantable_permissions_) << from << to
           >> perm_string;
        return true;
      } catch (const std::exception& e) {
        // TODO log
      }
      return false;
    }

    bool WsvSqliteDB::loadAccount(
        const std::string& account_id, std::string& domain_id, uint16_t& quorum
    ) {
      try {
        db_->getStatement(load_account_) << account_id
          >> std::tie(domain_id, quorum);
        return true;
      } catch (const std::exception& e) {
        // TODO log
      }
      return false;
    }

    bool WsvSqliteDB::loadAccountSignatories(
        const std::string& account_id,
        const std::function<void(const std::string &pk)>& callback
    ) {
      try {
        db_->getStatement(load_account_signatories_) << account_id
           >> callback;
        return true;
      } catch (const std::exception& e) {
        // TODO log
      }
      return false;
    }

    bool WsvSqliteDB::loadAccountRoles(
        const std::string& account_id,
        const std::function<void(const std::string &role_id)>& callback
    ) {
      try {
        db_->getStatement(load_account_roles_) << account_id
           >> callback;
        return true;
      } catch (const std::exception& e) {
        // TODO log
      }
      return false;
    }

    void WsvSqliteDB::dropPeers() {
      *db_ << "delete from peer";
    }

    void WsvSqliteDB::updateAccountAsset(
        const std::string &account_id, const std::string &asset_id,
        const std::string& balance, uint8_t precision
    ) {
      auto& stmt = db_->getStatement(update_account_asset_);
      stmt << account_id << asset_id << balance << precision;
      stmt++; // execute and reset
    }

    void WsvSqliteDB::addPeer(
        const std::string &private_key, const std::string &address
    ) {
      auto& stmt = db_->getStatement(add_peer_);
      stmt << private_key << address;
      stmt++; // execute and reset
    }

    void WsvSqliteDB::removePeer(const std::string &private_key) {
      auto& stmt = db_->getStatement(remove_peer_);
      stmt << private_key;
      stmt++; // execute and reset
    }

    void WsvSqliteDB::updateSignatory(
        const std::string &private_key, size_t count
    ) {
      auto& stmt = db_->getStatement(update_signatory_);
      stmt << private_key << count;
      stmt++; // execute and reset
    }

    void WsvSqliteDB::removeSignatory(const std::string &private_key) {
      auto& stmt = db_->getStatement(remove_signatory_);
      stmt << private_key;
      stmt++; // execute and reset
    }

    void WsvSqliteDB::addAccountSignatory(
        const std::string &account_id, const std::string &private_key
    ) {
      auto& stmt = db_->getStatement(add_account_signatory_);
      stmt << account_id << private_key;
      stmt++; // execute and reset
    }

    void WsvSqliteDB::removeAccountSignatory(
        const std::string &account_id, const std::string &private_key
    ) {
      auto& stmt = db_->getStatement(remove_account_signatory_);
      stmt << account_id << private_key;
      stmt++; // execute and reset
    }

    void WsvSqliteDB::appendRole(
        const std::string &role_id, const std::string &permissions
    ) {
      auto& stmt = db_->getStatement(append_role_);
      stmt << role_id << permissions;
      stmt++; // execute and reset
    }

    void WsvSqliteDB::createAccount(
        const std::string &account_id, const std::string &domain_id,
        uint16_t quorum
    ) {
      auto& stmt = db_->getStatement(create_account_);
      stmt << account_id << domain_id << quorum;
      stmt++; // execute and reset
    }

    void WsvSqliteDB::createAsset(
        const std::string &asset_id,
        const std::string& domain_id, uint8_t precision
    ) {
      auto& stmt = db_->getStatement(create_asset_);
      stmt << asset_id << domain_id << precision;
      stmt++; // execute and reset
    }

    void WsvSqliteDB::createDomain(
        const std::string &domain_id, const std::string &role_id
    ) {
      auto& stmt = db_->getStatement(create_domain_);
      stmt << domain_id << role_id;
      stmt++; // execute and reset
    }

    void WsvSqliteDB::attachAccountRole(
        const std::string &account_id, const std::string &role_id
    ) {
      auto& stmt = db_->getStatement(attach_account_role_);
      stmt << account_id << role_id;
      stmt++; // execute and reset
    }

    void WsvSqliteDB::detachAccountRole(
        const std::string &account_id, const std::string &role_id
    ) {
      auto& stmt = db_->getStatement(detach_account_role_);
      stmt << account_id << role_id;
      stmt++; // execute and reset
    }

    void WsvSqliteDB::updateGrantablePermissions(
        const std::string& from, const std::string& to,
        const std::string& permissions
    ) {
      auto& stmt = db_->getStatement(update_grantable_permissions_);
      stmt << from << to << permissions;
      stmt++; // execute and reset
    }

    void WsvSqliteDB::setQuorum(
        const std::string& account_id, uint16_t quorum
    ) {
      auto& stmt = db_->getStatement(set_quorum_);
      stmt << account_id << quorum;
      stmt++; // execute and reset
    }
    
    void WsvSqliteDB::createSchema() {
      static const char *prepare_tables_sql[] = {
          "CREATE TABLE IF NOT EXISTS ledger_state (\
            height INTEGER PRIMARY KEY,\
            hash TEXT NOT NULL)",
          "CREATE TABLE IF NOT EXISTS role (\
            role_id TEXT PRIMARY KEY,\
            permission BLOB NOT NULL)",
          "CREATE TABLE IF NOT EXISTS domain (\
            domain_id TEXT PRIMARY KEY,\
            default_role TEXT NOT NULL)",
          "CREATE TABLE IF NOT EXISTS signatory (\
            public_key TEXT PRIMARY KEY,\
            count INTEGER NOT NULL)" ,
          "CREATE TABLE IF NOT EXISTS peer (\
            public_key TEXT PRIMARY KEY,\
            address TEXT NOT NULL UNIQUE)",
          "CREATE TABLE IF NOT EXISTS asset (\
            asset_id TEXT PRIMARY KEY,\
            domain_id TEXT NOT NULL,\
            precision INTEGER NOT NULL)",
          "CREATE TABLE IF NOT EXISTS account (\
            account_id TEXT,\
            domain_id TEXT,\
            quorum INTEGER NOT NULL,\
            PRIMARY KEY (account_id))",
          "CREATE TABLE IF NOT EXISTS account_has_signatory (\
            account_id TEXT PRIMARY KEY,\
            public_key TEXT NOT NULL,\
            UNIQUE (account_id, public_key))",
          "CREATE TABLE IF NOT EXISTS account_has_asset (\
            account_id TEXT PRIMARY KEY,\
            asset_id TEXT NOT NULL,\
            amount TEXT NOT NULL,\
            precision INTEGER NOT NULL, \
            UNIQUE (account_id, asset_id))",
          "CREATE TABLE IF NOT EXISTS account_has_role (\
            account_id TEXT PRIMARY KEY,\
            role_id TEXT NOT NULL,\
            UNIQUE (account_id, role_id))",
          "CREATE TABLE IF NOT EXISTS account_has_grantable_permissions (\
            permittee_account_id TEXT NOT NULL,\
            account_id TEXT NOT NULL,\
            permission BLOB NOT NULL,\
            PRIMARY KEY (permittee_account_id, account_id))",
          "CREATE TABLE IF NOT EXISTS position_by_hash (\
            hash TEXT NOT NULL,\
            height INTEGER,\
            idx INTEGER)",
          "CREATE TABLE IF NOT EXISTS tx_status_by_hash (\
            hash TEXT NOT NULL,\
            status INTEGER)",
          "CREATE TABLE IF NOT EXISTS tx_position_by_creator (\
            creator_id TEXT,\
            height INTEGER,\
            idx INTEGER)",
          "CREATE TABLE IF NOT EXISTS position_by_account_asset (\
            account_id TEXT,\
            asset_id TEXT,\
            height INTEGER,\
            idx INTEGER)",
          "CREATE INDEX IF NOT EXISTS position_by_account_asset_index \
          ON position_by_account_asset\
          (account_id, asset_id, height, idx ASC)"};

      for (const char* sql : prepare_tables_sql) {
        *db_ << sql;
      }

      get_ledger_state_ = db_->createStatement(
          "SELECT height, hash FROM ledger_state \
          ORDER BY height DESC LIMIT 1"
      );

      load_account_assets_ = db_->createStatement(
         "SELECT asset_id, amount, precision FROM account_has_asset \
         WHERE account_id = ?"
      );

      load_grantable_permissions_ = db_->createStatement(
          "SELECT permission FROM account_has_grantable_permissions \
              WHERE permittee_account_id = ? AND account_id = ?"
      );
      load_account_ = db_->createStatement(
          "SELECT domain_id, quorum FROM account \
             WHERE account_id = ?"
      );

      load_account_signatories_ = db_->createStatement(
         "SELECT public_key FROM account_has_signatory \
          WHERE account_id = ?"
      );
      load_account_roles_ = db_->createStatement(
          "SELECT role_id FROM account_has_role \
          WHERE account_id = ?"
       );
       update_account_asset_ = db_->createStatement(
          "INSERT OR REPLACE INTO account_has_asset \
          VALUES(?, ?, ?, ?)"
       );
       add_peer_ = db_->createStatement(
          "INSERT INTO peer VALUES(?, ?)"
       );
       remove_peer_ = db_->createStatement(
          "DELETE FROM peer WHERE public_key = ?"
       );
       update_signatory_ = db_->createStatement(
          "INSERT OR REPLACE INTO signatory \
          VALUES(?, ?)"
       );
       remove_signatory_ = db_->createStatement(
          "DELETE FROM signatory WHERE public_key = ?"
       );
       add_account_signatory_ = db_->createStatement(
          "INSERT OR REPLACE INTO account_has_signatory \
          VALUES(?, ?)"
       );
       remove_account_signatory_ = db_->createStatement(
          "DELETE FROM account_has_signatory \
          WHERE account_id = ? AND public_key = ?"
       );
       append_role_ = db_->createStatement(
          "INSERT INTO role VALUES(?, ?)"
       );
       create_account_ = db_->createStatement(
          "INSERT INTO account VALUES(?, ?, ?)"
       );
       create_asset_ = db_->createStatement(
          "INSERT INTO asset VALUES(?, ?, ?)"
       );
       create_domain_ = db_->createStatement(
          "INSERT INTO domain VALUES(?, ?)"
       );
       attach_account_role_ = db_->createStatement(
          "INSERT INTO account_has_role VALUES(?, ?)"
       );
       detach_account_role_ = db_->createStatement(
          "DELETE FROM account_has_role \
          WHERE account_id = ? AND role_id = ?"
       );
       update_grantable_permissions_ = db_->createStatement(
          "INSERT OR REPLACE INTO account_has_grantable_permissions \
          VALUES(?, ?, ?)"
       );
       set_quorum_ = db_->createStatement(
          "UPDATE account SET quorum = ? WHERE account_id = ?"
       );

    }

  }  // namespace newstorage
}  // namespace iroha
