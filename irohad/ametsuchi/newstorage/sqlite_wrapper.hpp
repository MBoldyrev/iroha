/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SQLITE_WRAPPER_HPP
#define IROHA_SQLITE_WRAPPER_HPP

#include "sqlite_modern_cpp.h"

namespace iroha {
  namespace newstorage {
    class SqliteWrapper {
     public:
      static std::shared_ptr<SqliteWrapper> create(const std::string &db_file) {
        return std::shared_ptr<SqliteWrapper>(new SqliteWrapper(db_file));
      }

      template <typename T>
      SqliteWrapper &operator<<(const T &t) {
        db_ << t;
        return *this;
      }

      void attachMemoryDB(const char *name) {
        db_ << (std::string("attach database ':memory:' as ") + name);
      }

      void detachMemoryDB(const char *name) {
        db_ << (std::string("detach database ") + name);
      }

      template <size_t N>
      void createSchema(const char *(&sql)[N]) {
        for (size_t i = 0; i < N; ++i) {
          db_ << sql[N];
        }
      }

      sqlite::database_binder createStatement(const char *sql) {
        auto binder = db_ << sql;
        binder.used(true);  // TODO check that??
        return binder;
      }

      struct Transaction {
        explicit Transaction(SqliteWrapper &db) : db_(db) {}
        void commit() {
          db_ << "commit;";
        }
        ~Transaction() {
          db_ << "rollback;";
        }

       private:
        SqliteWrapper &db_;
      };

     private:
      explicit SqliteWrapper(const std::string &db_file) : db_(db_file)
      {}

      sqlite::database db_;
    };

    class PreparedStatement {
     public:
      static std::unique_ptr<PreparedStatement> create(sqlite::database_binder&& binder) {
        return std::make_unique<PreparedStatement>(std::move(binder));
      }
      explicit PreparedStatement(sqlite::database_binder&& binder) : binder_(std::move(binder))
      {}

      sqlite::database_binder& get() { return binder_; }
     private:
      sqlite::database_binder binder_;
    };
  }
}

#endif  // IROHA_SQLITE_WRAPPER_HPP
