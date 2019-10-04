/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_DB_TX_HPP
#define IROHA_DB_TX_HPP

#include <string>

namespace iroha {
  namespace newstorage {
    class SqliteWrapper;

    class DbTransaction {
     public:
      DbTransaction(const DbTransaction &) = delete;
      DbTransaction &operator=(const DbTransaction &) = delete;
      DbTransaction(DbTransaction &&r) noexcept : db_(r.db_) {
        r.db_ = nullptr;
      }
      DbTransaction &operator=(DbTransaction &&r) noexcept {
        db_ = r.db_;
        r.db_ = nullptr;
        return *this;
      }
      DbTransaction() : db_(nullptr) {}
      explicit DbTransaction(SqliteWrapper &db);

      void commit();

      void rollback();

      ~DbTransaction() {
        rollback();
      }

     private:
      SqliteWrapper *db_;
    };

    struct DbSavepoint {
      DbSavepoint(const DbSavepoint &) = delete;
      DbSavepoint &operator=(const DbSavepoint &) = delete;
      DbSavepoint(DbSavepoint &&) = default;
      DbSavepoint &operator=(DbSavepoint &&) = default;
      DbSavepoint(SqliteWrapper &db, std::string name);

      void release();

      void rollback();

      ~DbSavepoint() {
        rollback();
      }

     private:
      SqliteWrapper &db_;
      std::string name_;
    };
  }  // namespace newstorage
}  // namespace iroha

#endif  // IROHA_DB_TX_HPP
