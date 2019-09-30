/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ametsuchi/newstorage/wsv_sqlite_db.hpp"
#include "ametsuchi/newstorage/sqlite_wrapper.hpp"
//#include <boost/filesystem/operations.hpp>
#include "sqlite_modern_cpp.h"
#include "logger/logger.hpp"

#include <iostream>

namespace iroha {
  namespace newstorage {

     WsvSqliteDB::WsvSqliteDB(std::shared_ptr<SqliteWrapper> db)
        : db_(std::move(db))
    {
      createSchema();
      uint64_t height = 0;
      std::string hash;
      if (getLedgerState(height, hash)) {
        db_->log("Wsv DB opened, ledger state: {} - {}", height, hash);
      } else {
        db_->log("Wsv DB opened, zero ledger state");
      }
    }

    std::string WsvSqliteDB::getLastError() {
      return db_->getErrMsg();
    }

    void WsvSqliteDB::loadRoles(
        const std::function<void(const std::string& role, const std::string& permissions)>&
        callback)
    {
      execQuery(db_, load_roles_, callback);
    }

    void WsvSqliteDB::loadDomains(
        const std::function<
            void(const std::string& domain, const std::string& role)>&
        callback
    ) {
      execQuery(db_, load_domains_, callback);
    }

    void WsvSqliteDB::loadAllSignatories(
        const std::function<void(const std::string& signatory)>& callback
    ) {
      execQuery(db_, load_all_signatories_, callback);
    }

    void WsvSqliteDB::loadPeers(
        const std::function<void(const std::string& pk, const std::string& address)>& callback
    ) {
      execQuery(db_, load_peers_, callback);
    }

    void WsvSqliteDB::loadAssets(
        const std::function<
            void(const std::string& id, const std::string& domain, uint8_t precision)>&
        callback
    ) {
      execQuery(db_, load_assets_, callback);
    }

    bool WsvSqliteDB::getLedgerState(uint64_t &height, std::string &hash) {
      height = 0;
      hash.clear();
      auto callback = [&](const uint64_t &_height, const std::string &_hash) {
        height = _height; hash = _hash;
      };
      return execQueryNoThrow(db_,
          get_ledger_state_, callback)
          && height > 0;
    }

    bool WsvSqliteDB::peerExists(const std::string &pk) {
      int count = -1;
      return execQueryNoThrow(db_, peer_exists_,
          count, pk) && (count > 0);
    }

    bool WsvSqliteDB::loadAccountAssets(
        const std::string& account_id,
        const std::function<
            void(const std::string& asset_id, const std::string& balance, uint8_t precision)>&
        callback
    ) {
      return execQueryNoThrow(db_,
          load_account_assets_, callback, account_id);
    }

    bool WsvSqliteDB::loadAccountPermissions(
        const std::string& account_id, std::string& perm_string
    ) {
      perm_string.clear();
      return execQueryNoThrow(db_,
                              load_account_permissions_,
                              perm_string, account_id) && !perm_string.empty();
    }

    bool WsvSqliteDB::loadGrantablePermissions(
        const std::string& from, const std::string& to, std::string& perm_string
    ) {
      perm_string.clear();
      return execQueryNoThrow(db_,
          load_grantable_permissions_,
          perm_string, from, to) && !perm_string.empty();
    }

    bool WsvSqliteDB::loadAccount(
        const std::string& account_id, std::string& domain_id, uint16_t& quorum,
        std::string& perm_string
    ) {
      domain_id.clear();
      auto callback = [&](const std::string& _domain_id, const uint16_t& _quorum,
                          const std::string& _perm_string) {
        domain_id = _domain_id; quorum = _quorum; perm_string = _perm_string;
      };
      return execQueryNoThrow(db_,
          load_account_,
          callback, account_id)
          && !domain_id.empty();
    }

    bool WsvSqliteDB::loadAccountSignatories(
        const std::string& account_id,
        const std::function<void(const std::string &pk)>& callback
    ) {
      return execQueryNoThrow(db_,
          load_account_signatories_, callback, account_id);
    }

    bool WsvSqliteDB::loadAccountRoles(
        const std::string& account_id,
        const std::function<void(const std::string &role_id)>& callback
    ) {
      return execQueryNoThrow(db_,
          load_account_roles_, callback, account_id);
    }

    int WsvSqliteDB::dropPeers() {
      return execCommand(db_, drop_peers_);
    }

    int WsvSqliteDB::updateAccountAsset(
        const std::string &account_id, const std::string &asset_id,
        const std::string& balance, uint8_t precision
    ) {
      return execCommand(db_, update_account_asset_,
          account_id, asset_id, balance, precision);
    }

    int WsvSqliteDB::setLedgerState(uint64_t height, const std::string& hash) {
      return execCommand(db_, set_ledger_state_, height, hash);
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
            PRIMARY KEY (permittee_account_id, account_id)))"};

      try {
        *db_ << "PRAGMA foreign_keys = ON";

        SqliteWrapper::Transaction tx(*db_);

        for (const char *sql : prepare_tables_sql) {
          *db_ << sql;
        }

        get_ledger_state_ = db_->createStatement(
            R"(SELECT height, hash FROM ledger_state
            ORDER BY height DESC LIMIT 1)");

        load_roles_ = db_->createStatement("SELECT * FROM role");

        load_domains_ = db_->createStatement("SELECT * FROM domain");

        load_all_signatories_ = db_->createStatement(
            R"(SELECT DISTINCT(public_key) FROM peer
            UNION SELECT DISTINCT(public_key) from account_has_signatory)");

        load_peers_ = db_->createStatement("SELECT * FROM peer");

        load_assets_ = db_->createStatement("SELECT * FROM asset");

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

        set_ledger_state_ =
            db_->createStatement("INSERT INTO ledger_state VALUES(?, ?)");

        add_peer_ = db_->createStatement("INSERT INTO peer VALUES(?, ?)");

        remove_peer_ =
            db_->createStatement("DELETE FROM peer WHERE public_key = ?");

        drop_peers_ = db_->createStatement("DELETE FROM peer");

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

        tx.commit();
      } catch (sqlite::sqlite_exception &e) {
        db_->log("Wsv DB: error in createSchema, sql={}", e.get_sql());
        throw;
      }
    }

  }  // namespace newstorage
}  // namespace iroha
