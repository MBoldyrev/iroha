/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_KEYVALUE_DB_BACKEND_HPP
#define IROHA_KEYVALUE_DB_BACKEND_HPP

#include <leveldb/slice.h>
#include <functional>
#include <vector>
#include "logger/logger_fwd.hpp"

namespace leveldb { class DB; }

namespace iroha {
  namespace newstorage {

    using leveldb::Slice;

    inline Slice slice(const void* d, size_t n) { return Slice((const char*)d, n); }

    inline Slice slice(const std::vector<uint8_t>& v) {
      return Slice((const char*)v.data(), v.size());
    }

    class KeyValueDbBackend {
     public:
      struct Options {
        std::string path;
        char delimiter = 0x01;
        //bool uint64keys = false;
        // ~etc
      };

      KeyValueDbBackend(Options options, logger::LoggerPtr log);

      void put(const Slice& key, const Slice& value);
      void put(const Slice& key_scope, const Slice& key, const Slice& value);
      void put(uint64_t key, const Slice& value);

      std::string get(const Slice& key);
      std::string get(const Slice& key_scope, const Slice& key);
      std::string get(uint64_t key);

      void iterate_from(const Slice& key,
          const std::function<bool(const Slice& key, const Slice& value)>& fn);
      void iterate_from(uint64_t key,
          const std::function<bool(uint64_t key, const Slice& value)>& fn);
      void iterate_scope(const Slice& key_scope,
          const std::function<bool(const Slice& key_scope, const Slice& key, const Slice& value)>& fn);

      // ~etc

     private:

      Options options_;
      std::shared_ptr<leveldb::DB> db_;
      logger::LoggerPtr log_;
    };

  }  // namespace newstorage
}  // namespace iroha

#endif //IROHA_KEYVALUE_DB_BACKEND_HPP
