/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_KEYVALUE_DB_BACKEND_HPP
#define IROHA_KEYVALUE_DB_BACKEND_HPP

#include <string>
#include <functional>
#include "logger/logger_fwd.hpp"

namespace leveldb { class DB; }

namespace iroha {
  namespace newstorage {

    class KeyValueDbBackend {
     public:
      struct Options {
        std::string path;
        char delimiter = 0x01;
        // ~etc
      };

      struct Blob {
        const void* data = nullptr;
        size_t size = 0;
      };

      KeyValueDbBackend(Options options, logger::LoggerPtr log);

      void put(Blob key, Blob value);
      void put(const std::string& key, Blob value);
      void put(const std::string& key_scope, const std::string& key, Blob value);

      std::string get(Blob key);
      std::string get(const std::string& key);
      std::string get(const std::string& key_scope, const std::string& key);

      void iterate_from(const std::string& key,
          std::function<bool(const Blob& key, const Blob& value)> fn);
      void iterate_scope(const std::string& key,
          std::function<bool(const Blob& key_scope, const Blob& key, const Blob& value)> fn);

      // ~etc

     private:

      Options options_;
      std::shared_ptr<leveldb::DB> db_;
      logger::LoggerPtr log_;
    };

  }  // namespace newstorage
}  // namespace iroha

#endif //IROHA_KEYVALUE_DB_BACKEND_HPP
