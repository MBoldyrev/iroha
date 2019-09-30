/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ametsuchi/newstorage/block_index_db.hpp"
#include "ametsuchi/newstorage/sqlite_wrapper.hpp"
//#include <boost/filesystem/operations.hpp>
#include "sqlite_modern_cpp.h"

#include <iostream>

namespace iroha {
  namespace newstorage {

    BlockIndexDB::BlockIndexDB(std::shared_ptr<SqliteWrapper> db)
        : db_(std::move(db))
    {
      createSchema();
      uint64_t height = 0;
      std::string hash;
      if (getLedgerState(height, hash)) {
        db_->log("Block index DB opened, ledger state: {} - {}", height, hash);
      } else {
        db_->log("Block index DB opened, zero ledger state");
      }
    }

    std::string BlockIndexDB::getLastError() {
      return db_->getErrMsg();
    }

    void BlockIndexDB::beginTx() { db_->beginTx(); }
    void BlockIndexDB::commitTx() { db_->commitTx(); }
    void BlockIndexDB::rollbackTx() { db_->rollbackTx(); }

    bool BlockIndexDB::getLedgerState(uint64_t &height, std::string &hash) {
      height = 0;
      hash.clear();
      auto callback = [&](const uint64_t &_height, const std::string &_hash) {
        height = _height; hash = _hash;
      };
      return execQueryNoThrow(db_,get_ledger_state_, callback)
          && height > 0;
    }

    int BlockIndexDB::setLedgerState(uint64_t height, const std::string& hash) {
      return execCommand(db_, set_ledger_state_, height, hash);
    }

    int BlockIndexDB::insertPositionByHash(
        const std::string& hash, uint64_t height, size_t idx)
    {
      return execCommand(db_, insert_position_by_hash_,
          hash, height, idx);
    }

    int BlockIndexDB::insertStatusByHash(const std::string& hash, int status) {
      return execCommand(db_, insert_status_by_hash_,
                         hash, status);
    }

    int BlockIndexDB::insertPositionByCreator(const std::string& creator_id,
                                uint64_t height, size_t idx)
    {
      return execCommand(db_, insert_position_by_creator_,
                         creator_id, height, idx);
    }

    int BlockIndexDB::insertPositionByAccountAsset(
        const std::string& account_id,
        const std::string& asset_id, uint64_t height, size_t idx)
    {
      return execCommand(db_, insert_position_by_account_asset_,
          account_id, asset_id, height, idx);
    }

    void BlockIndexDB::createSchema() {
      static const char *prepare_tables_sql[] = {
          R"(CREATE TABLE IF NOT EXISTS ledger_state (
            height INTEGER PRIMARY KEY,
            hash TEXT NOT NULL))",
          R"(CREATE TABLE IF NOT EXISTS tx_position_by_hash (
            hash TEXT PRIMARY KEY,
            height INTEGER NOT NULL,
            idx INTEGER NOT NULL))",
          R"(CREATE INDEX IF NOT EXISTS hash2pos ON tx_position_by_hash
            (hash, height, idx ASC))",
          R"(CREATE TABLE IF NOT EXISTS tx_status_by_hash (
            hash TEXT PRIMARY KEY,
            status INTEGER NOT NULL))",
          R"(CREATE TABLE IF NOT EXISTS tx_position_by_creator (
            creator_id TEXT NOT NULL,
            height INTEGER NOT NULL,
            idx INTEGER NOT NULL))",
          R"(CREATE INDEX IF NOT EXISTS creator2pos on tx_position_by_creator
            (creator_id, height, idx ASC))",
          R"(CREATE TABLE IF NOT EXISTS tx_position_by_account_asset (
            account_id TEXT NOT NULL,
            asset_id TEXT NOT NULL,
            height INTEGER NOT NULL,
            idx INTEGER NOT NULL))",
          R"(CREATE INDEX IF NOT EXISTS aa2pos ON tx_position_by_account_asset
            (account_id, asset_id, height, idx ASC))"
      };

      try {
        SqliteWrapper::Transaction tx(*db_);

        for (const char *sql : prepare_tables_sql) {
          *db_ << sql;
        }

        get_ledger_state_ = db_->createStatement(
            R"(SELECT height, hash FROM ledger_state
            ORDER BY height DESC LIMIT 1)");

        set_ledger_state_ = db_->createStatement(
            "INSERT INTO ledger_state VALUES(?, ?)");

        insert_position_by_hash_ = db_->createStatement(
            R"(INSERT INTO tx_position_by_hash
            (hash, height, idx) VALUES(?,?,?))"
        );

        insert_status_by_hash_ = db_->createStatement(
            "(INSERT INTO tx_status_by_hash (hash, status) VALUES(?, ?)"
        );

        insert_position_by_creator_ = db_->createStatement(
            R"(INSERT INTO tx_position_by_creator
            (creator_id, height, idx) VALUES (?, ?, ?))"
        );

        insert_position_by_account_asset_ = db_->createStatement(
           R"(INSERT INTO tx_position_by_account_asset
             (account_id, asset_id, height, idx) VALUES(?,?,?,?))"
        );

        tx.commit();
      } catch (sqlite::sqlite_exception& e) {
        db_->log("error in createSchema, sql={}", e.get_sql());
        throw;
      }
     }

  }  // namespace newstorage
}  // namespace iroha
