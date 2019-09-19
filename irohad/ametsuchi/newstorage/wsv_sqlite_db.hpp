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

    class WsvSqliteDB {
     public:
      WsvSqliteDB(const std::string &db_file, logger::LoggerPtr log);

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

      /*
      void getSignatories(const std::string &account_id,
                          std::function<void(const std::string &)> fn);

      void getPeers(
          std::function<void(const std::string &, const std::string &)> fn);

      void insertPeer(const std::string &pub_key, const std::string &address);

      void dropPeers();

      void dropAll();

      int getTxStatusByHash(const std::string &hash);
      */

     private:
      void createSchema();

      std::shared_ptr<SqliteWrapper> db_;
      enum Statements {
        get_n_by_writer,
        get_n_by_account,
        get_n_by_writer_account,
        get_n_by_account_key,
        inc_writer,
        inc_account,
        inc_writer_account,
        inc_account_key,

        statements_count
      };
      std::unique_ptr<PreparedStatement> statements_[statements_count];
      std::string path_;
      std::vector<char> buffer_;
      logger::LoggerPtr log_;
    };
  }  // namespace newstorage
}  // namespace iroha

#endif  // IROHA_WSV_SQLITE_DB_HPP
