/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ametsuchi/newstorage/wsv_sqlite_db.hpp"
#include "ametsuchi/newstorage/sqlite_wrapper.hpp"
//#include <boost/filesystem/operations.hpp>
#include "sqlite_modern_cpp.h"
//#include "logger/logger.hpp"

#include <iostream>

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
      *db_ << R"(SELECT DISTINCT(public_key) FROM peer
          UNION SELECT DISTINCT(public_key) from account_has_signatory)"
          >> callback;
    }

    void WsvSqliteDB::loadPeers(
        const std::function<void(const std::string& pk, const std::string& address)>& callback
    ) {
      *db_ << "SELECT * from peer" >> callback;
    }

    bool WsvSqliteDB::peerExists(const std::string &pk) {
      int count = -1;
      db_->getStatement(peer_exists_) << pk >> count;
      return (count > 0);
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

    bool WsvSqliteDB::loadAccountPermissions(
        const std::string& account_id, std::string& perm_string
    ) {
      try {
        db_->getStatement(load_account_permissions_)
          << account_id >> perm_string;
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

    int WsvSqliteDB::dropPeers() {
      int rows_affected = -1;
      *db_ << "delete from peer" >> rows_affected;
      return rows_affected;
    }

    namespace {
      inline sqlite::database_binder &bindArgs(
          sqlite::database_binder &stmt) {
        return stmt;
      }

      template <typename Arg, typename... Args>
      inline sqlite::database_binder &bindArgs(
          sqlite::database_binder &stmt, const Arg &arg, const Args &... args) {
        stmt << arg;
        return bindArgs(stmt, args...);
      }

      template <typename... Args>
      inline int execCommand(std::shared_ptr<SqliteWrapper> &db,
                                    StatementHandle st_handle,
                                    const Args &... args) {
        int rows_affected = -1;
        auto &stmt = db->getStatement(st_handle);
        bindArgs(stmt, args...);
        stmt >> rows_affected;
        return rows_affected;
      }
    } //namespace

    int WsvSqliteDB::updateAccountAsset(
        const std::string &account_id, const std::string &asset_id,
        const std::string& balance, uint8_t precision
    ) {
      return execCommand(db_, update_account_asset_,
          account_id, asset_id, balance, precision);
    }

    int WsvSqliteDB::addPeer(
        const std::string &public_key, const std::string &address
    ) {
      return execCommand(db_, add_peer_, public_key, address);
    }

    int WsvSqliteDB::removePeer(const std::string &public_key) {
      return execCommand(db_, remove_peer_, public_key);
    }

    int WsvSqliteDB::addAccountSignatory(
        const std::string &account_id, const std::string &public_key
    ) {
      return execCommand(db_, add_account_signatory_,
          account_id, public_key);
    }

    int WsvSqliteDB::removeAccountSignatory(
        const std::string &account_id, const std::string &public_key
    ) {
      return execCommand(db_, remove_account_signatory_,
                         account_id, public_key);
    }

    int WsvSqliteDB::createRole(
        const std::string &role_id, const std::string &permissions
    ) {
      return execCommand(db_, create_role_,
                         role_id, permissions);
    }

    int WsvSqliteDB::createAccount(
        const std::string &account_id, const std::string &domain_id,
        uint16_t quorum
    ) {
      return execCommand(db_, create_account_,
                        account_id, domain_id, quorum, domain_id);
    }

    int WsvSqliteDB::createAsset(
        const std::string &asset_id,
        const std::string& domain_id, uint8_t precision
    ) {
      return execCommand(db_, create_asset_,
          asset_id, domain_id, precision);
    }

    int WsvSqliteDB::createDomain(
        const std::string &domain_id, const std::string &role_id
    ) {
      return execCommand(db_, create_domain_,
                         domain_id, role_id);
    }

    int WsvSqliteDB::attachAccountRole(
        const std::string &account_id, const std::string &role_id
    ) {
      return execCommand(db_, attach_account_role_,
          account_id, role_id);
    }

    int WsvSqliteDB::detachAccountRole(
        const std::string &account_id, const std::string &role_id
    ) {
      return execCommand(db_, detach_account_role_,
          account_id, role_id);
    }

    int WsvSqliteDB::updateGrantablePermissions(
        const std::string& from, const std::string& to,
        const std::string& permissions
    ) {
      return execCommand(db_, update_grantable_permissions_,
          from, to, permissions);
    }

    int WsvSqliteDB::setQuorum(
        const std::string& account_id, uint16_t quorum
    ) {
      return execCommand(db_, set_quorum_, quorum, account_id);
    }

    int WsvSqliteDB::updateAccountPermissions(
        const std::string& account_id, const std::string& permissions
    ) {
      return execCommand(db_, update_permissions_, permissions, account_id);
    }

    void WsvSqliteDB::createSchema() {
      static const char *prepare_tables_sql[] = {
          "PRAGMA foreign_keys = ON",
          "PRAGMA count_changes = ON",
          R"(CREATE TABLE IF NOT EXISTS ledger_state (
            height INTEGER PRIMARY KEY,
            hash TEXT NOT NULL))",
          R"(CREATE TABLE IF NOT EXISTS role (
            role_id TEXT PRIMARY KEY,
            permission TEXT NOT NULL))",
          R"(CREATE TABLE IF NOT EXISTS domain (
            domain_id TEXT PRIMARY KEY,
            default_role TEXT REFERENCES role(role_id)))",
          R"(CREATE TABLE IF NOT EXISTS peer (
            public_key TEXT PRIMARY KEY,
            address TEXT NOT NULL UNIQUE))",
          R"(CREATE TABLE IF NOT EXISTS asset (
            asset_id TEXT PRIMARY KEY,
            domain_id TEXT REFERENCES domain(domain_id),
            precision INTEGER NOT NULL))",
          R"(CREATE TABLE IF NOT EXISTS account (
            account_id TEXT PRIMARY KEY,
            domain_id TEXT REFERENCES domain(domain_id),
            quorum INTEGER NOT NULL,
            permission TEXT NOT NULL))",
          R"(CREATE TABLE IF NOT EXISTS account_has_signatory (
            account_id TEXT REFERENCES account(account_id),
            public_key TEXT NOT NULL,
            UNIQUE (account_id, public_key)))",
          R"(CREATE INDEX IF NOT EXISTS acc_i1 on account_has_signatory
            (account_id))",
          R"(CREATE INDEX IF NOT EXISTS sig_i1 on account_has_signatory
            (public_key))",
          R"(CREATE TABLE IF NOT EXISTS account_has_asset (
            account_id TEXT REFERENCES account(account_id),
            asset_id TEXT,
            amount TEXT NOT NULL,
            precision INTEGER NOT NULL,
            UNIQUE (account_id, asset_id)))",
          R"(CREATE INDEX IF NOT EXISTS acc_a2 on account_has_asset
            (account_id))",
          R"(CREATE TABLE IF NOT EXISTS account_has_role (
            account_id TEXT REFERENCES account(account_id),
            role_id TEXT REFERENCES role(role_id),
            UNIQUE (account_id, role_id)))",
          R"(CREATE TABLE IF NOT EXISTS account_has_grantable_permissions (
            permittee_account_id TEXT REFERENCES account(account_id),
            account_id TEXT REFERENCES account(account_id),
            permission TEXT NOT NULL,
            PRIMARY KEY (permittee_account_id, account_id)))"
            /*,
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
          (account_id, asset_id, height, idx ASC)"*/};

      try {
        for (const char *sql : prepare_tables_sql) {
          *db_ << sql;
        }

        get_ledger_state_ = db_->createStatement(
            R"(SELECT height, hash FROM ledger_state
            ORDER BY height DESC LIMIT 1)");

        load_account_assets_ = db_->createStatement(
            R"(SELECT asset_id, amount, precision FROM account_has_asset
           WHERE account_id = ?)");

        load_grantable_permissions_ = db_->createStatement(
            R"(SELECT permission FROM account_has_grantable_permissions
                WHERE permittee_account_id = ? AND account_id = ?)");

        load_account_ = db_->createStatement(
            R"(SELECT domain_id, quorum, permission FROM account
               WHERE account_id = ?)");

        load_account_permissions_ = db_->createStatement(
            R"(SELECT permission FROM account WHERE account_id = ?)");

        load_account_signatories_ = db_->createStatement(
            R"(SELECT public_key FROM account_has_signatory
            WHERE account_id = ?)");

        load_account_roles_ = db_->createStatement(
            R"(SELECT role_id FROM account_has_role
            WHERE account_id = ?)");

        peer_exists_ = db_->createStatement(
            R"(SELECT count(*) FROM peer WHERE public_key = ?)");

        add_peer_ = db_->createStatement("INSERT INTO peer VALUES(?, ?)");

        remove_peer_ =
            db_->createStatement("DELETE FROM peer WHERE public_key = ?");

        add_account_signatory_ = db_->createStatement(
            R"(INSERT INTO account_has_signatory VALUES(?, ?))");

        remove_account_signatory_ = db_->createStatement(
            R"(DELETE FROM account_has_signatory
            WHERE account_id = ? AND public_key = ?)");

        create_role_ = db_->createStatement("INSERT INTO role VALUES(?, ?)");

        create_account_ = db_->createStatement(
            R"(INSERT INTO account VALUES(
                ?, ?, ?,
                (SELECT permission FROM role WHERE role_id =
                (SELECT default_role FROM domain WHERE domain_id = ?))))");

        create_asset_ =
            db_->createStatement("INSERT INTO asset VALUES(?, ?, ?)");

        create_domain_ =
            db_->createStatement("INSERT INTO domain VALUES(?, ?)");

        attach_account_role_ =
            db_->createStatement("INSERT INTO account_has_role VALUES(?, ?)");

        detach_account_role_ = db_->createStatement(
            R"(DELETE FROM account_has_role
            WHERE account_id = ? AND role_id = ?)");

        update_grantable_permissions_ = db_->createStatement(
            R"(INSERT OR REPLACE INTO account_has_grantable_permissions
            VALUES(?, ?, ?))");

        set_quorum_ = db_->createStatement(
            "UPDATE account SET quorum = ? WHERE account_id = ?");

        update_permissions_ = db_->createStatement(
            "UPDATE account SET permission = ? WHERE account_id = ?");

      } catch (sqlite::sqlite_exception& e) {
        std::cerr  << e.get_code() << ": " << e.what()
                << " : " << db_->getErrMsg() << " during "
                 << e.get_sql() << "\n";
      }
     }

  }  // namespace newstorage
}  // namespace iroha
