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

      void loadAssets(
          const std::function<
              void(const std::string& id, const std::string& domain, uint8_t precision)>&
          callback
      );

      void loadAccountAssets(
          const std::string& account_id,
          const std::function<
              void(const std::string& asset_id, const std::string& balance, uint8_t precision)>&
          callback
      );

      bool loadGrantablePermissions(
          const std::string& from, const std::string& to, std::string& perm_string
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

      std::string path_;
      std::vector<char> buffer_;
      logger::LoggerPtr log_;
    };
  }  // namespace newstorage
}  // namespace iroha

#endif  // IROHA_WSV_SQLITE_DB_HPP
