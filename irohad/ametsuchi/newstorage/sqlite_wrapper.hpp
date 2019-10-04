/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SQLITE_WRAPPER_HPP
#define IROHA_SQLITE_WRAPPER_HPP

#include "logger/logger.hpp"
#include "sqlite_modern_cpp.h"

namespace iroha {
  namespace newstorage {
    class SqliteWrapper {
     public:
      using StatementHandle = size_t;

      struct Options {
        std::string db_file;
        std::string log_prefix;
        logger::LogLevel log_level = logger::LogLevel::kDebug;

        Options() = default;
        Options(std::string _db_file,
                std::string _log_prefix,
                logger::LogLevel _log_level = logger::LogLevel::kDebug)
            : db_file(std::move(_db_file)),
              log_prefix(std::move(_log_prefix)),
              log_level(_log_level) {}
      };

      static std::shared_ptr<SqliteWrapper> create(const Options &options,
                                                   logger::LoggerPtr log) {
        return std::shared_ptr<SqliteWrapper>(
            new SqliteWrapper(options, std::move(log)));
      }

      ~SqliteWrapper() {
        // to prevent them from execution while going out of scope
        for (auto &st : statements_) {
          st.used(true);
        }
      }

      template <typename T>
      auto operator<<(const T &t) {
        return (db_ << t);
      }

      template <typename T>
      SqliteWrapper &operator>>(T &t) {
        db_ >> t;
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

      int getErrCode() {
        return sqlite3_extended_errcode(db_.connection().get());
      }

      std::string getErrMsg() {
        int ec = getErrCode();
        return (ec == 0) ? std::string()
                         : log_prefix_ + std::string(sqlite3_errstr(ec)) + ": "
                + sqlite3_errmsg(db_.connection().get());
      }

      template <typename... Args>
      void log(const std::string &format, const Args &... args) {
        if (log_)
          log_->log(log_level_, format, args...);
      }

      void logErrMsg() {
        if (log_)
          log(getErrMsg());
      }

      StatementHandle createStatement(const char *sql) {
        StatementHandle h = statements_.size();
        statements_.emplace_back(db_ << sql);
        return h;
      }

      sqlite::database_binder &getStatement(StatementHandle h) {
        if (h >= statements_.size()) {
          throw std::runtime_error("SqliteWrapper: invalid statement");
        }
        return statements_[h];
      }

      void beginTx() {
        db_ << "begin";
      }
      void commitTx() {
        db_ << "commit";
      }
      void rollbackTx() {
        db_ << "rollback";
      }

      void createSavepoint(const std::string &name) {
        db_ << std::string("savepoint ") + name;
      }
      void releaseSavepoint(const std::string &name) {
        db_ << std::string("release savepoint ") + name;
      }
      void rollbackToSavepoint(const std::string &name) {
        db_ << std::string("rollback to savepoint ") + name;
      }

     private:
      explicit SqliteWrapper(const Options &options, logger::LoggerPtr logger)
          : db_(options.db_file),
            log_prefix_(options.log_prefix),
            log_level_(options.log_level),
            log_(std::move(logger)) {
        db_ << "PRAGMA count_changes = ON";
      }

      sqlite::database db_;
      std::string log_prefix_;
      logger::LogLevel log_level_;
      logger::LoggerPtr log_;
      std::vector<sqlite::database_binder> statements_;
    };

    inline sqlite::database_binder &bindArgs(sqlite::database_binder &stmt) {
      return stmt;
    }

    template <typename Arg, typename... Args>
    inline sqlite::database_binder &bindArgs(sqlite::database_binder &stmt,
                                             const Arg &arg,
                                             const Args &... args) {
      stmt << arg;
      return bindArgs(stmt, args...);
    }

    template <typename... Args>
    inline int execCommand(std::shared_ptr<SqliteWrapper> &db,
                           SqliteWrapper::StatementHandle st_handle,
                           const Args &... args) {
      try {
        // DB must have  "PRAGMA count_changes = ON",
        int rows_affected = -1;
        auto &stmt = db->getStatement(st_handle);
        bindArgs(stmt, args...);
        stmt >> rows_affected;
        return rows_affected;
      } catch (...) {
        db->logErrMsg();
      }
      return -1;
    }

    template <typename Sink, typename... Args>
    inline bool execQueryNoThrow(std::shared_ptr<SqliteWrapper> &db,
                                 SqliteWrapper::StatementHandle st_handle,
                                 Sink &&sink,
                                 const Args &... args) {
      try {
        auto &stmt = db->getStatement(st_handle);
        bindArgs(stmt, args...);
        stmt >> sink;
        return true;
      } catch (...) {
        db->logErrMsg();
      }
      return false;
    }

    template <typename Sink, typename... Args>
    inline void execQuery(std::shared_ptr<SqliteWrapper> &db,
                          SqliteWrapper::StatementHandle st_handle,
                          Sink &&sink,
                          const Args &... args) {
      if (!execQueryNoThrow(db, st_handle, sink, args...)) {
        throw std::runtime_error(db->getErrMsg());
      }
    }

  }  // namespace newstorage
}  // namespace iroha

#endif  // IROHA_SQLITE_WRAPPER_HPP
