/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_WSV_SQLITE_DB_HPP
#define IROHA_WSV_SQLITE_DB_HPP

#include <functional>
#include <string>
#include "logger/logger_fwd.hpp"

namespace iroha {
  namespace newstorage {
    class SqliteWrapper;

    class WsvSqliteDB {
     public:
      WsvSqliteDB(const std::string &db_file, logger::LoggerPtr log);

      void getSignatories(const std::string &account_id,
                          std::function<void(const std::string &)> fn);

      void getPeers(
          std::function<void(const std::string &, const std::string &)> fn);

      void insertPeer(const std::string &pub_key, const std::string &address);

      void dropPeers();

      void dropAll();

      int getTxStatusByHash(const std::string &hash);

     private:
      void createSchema();

      std::shared_ptr<SqliteWrapper> db_;
      std::string path_;
      logger::LoggerPtr log_;
    };
  }  // namespace newstorage
}  // namespace iroha

#endif  // IROHA_WSV_SQLITE_DB_HPP
