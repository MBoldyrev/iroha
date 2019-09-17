/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_KEYVALUE_DB_BACKEND_HPP
#define IROHA_KEYVALUE_DB_BACKEND_HPP

#include <leveldb/slice.h>
#include <functional>
#include <memory>
#include <vector>

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

      void create(Options options);

      void put(const Slice& key, const Slice& value);
      void put(const Slice& key_scope, const Slice& key, const Slice& value);
      void put(uint64_t key, const Slice& value);

      bool keyExists(const Slice& key) const;

      std::string get(const Slice& key) const;
      std::string get(const Slice& key_scope, const Slice& key) const;
      std::string get(uint64_t key) const;

      bool get_last_num_key(uint64_t& key);

      void iterate_from(const Slice& key,
          const std::function<bool(const Slice& key, const Slice& value)>& fn) const;
      void iterate_from(uint64_t key,
          const std::function<bool(uint64_t key, const Slice& value)>& fn) const;
      void iterate_scope(const Slice& key_scope,
          const std::function<bool(const Slice& key_scope, const Slice& key, const Slice& value)>& fn) const;

      // ~etc

      struct WriteBatch {
        WriteBatch& put(const Slice& key, const Slice& value) {
          put_ops.emplace_back(key, value);
          return *this;
        }
        WriteBatch& del(const Slice& key) {
          del_ops.emplace_back(key);
          return *this;
        }
        std::vector<std::pair<Slice, Slice>> put_ops;
        std::vector<Slice> del_ops;
      };

      bool apply(WriteBatch& batch);

     private:

      Options options_;
      std::shared_ptr<leveldb::DB> db_;
    };

  }  // namespace newstorage
}  // namespace iroha

#endif //IROHA_KEYVALUE_DB_BACKEND_HPP
