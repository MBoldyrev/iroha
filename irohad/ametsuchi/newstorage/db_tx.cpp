/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "db_tx.hpp"
#include "sqlite_wrapper.hpp"

namespace iroha {
  namespace newstorage {
    DbTransaction::DbTransaction(SqliteWrapper &db) : db_(&db) {
      db_->beginTx();
    }

    void DbTransaction::commit() {
      if (db_) {
        db_->commitTx();
        db_ = nullptr;
      }
    }

    void DbTransaction::rollback() {
      if (db_) {
        db_->rollbackTx();
        db_ = nullptr;
      }
    }

    DbSavepoint::DbSavepoint(SqliteWrapper &db, std::string name)
        : db_(db), name_(std::move(name)) {
      db_.createSavepoint(name_);
    }

    void DbSavepoint::release() {
      db_.releaseSavepoint(name_);
      name_.clear();
    }

    void DbSavepoint::rollback() {
      if (!name_.empty()) {
        db_.rollbackToSavepoint(name_);
        name_.clear();
      }
    }
  }  // namespace newstorage
} // namespace iroha