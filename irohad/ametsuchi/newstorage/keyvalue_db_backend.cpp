/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <leveldb/db.h>
#include <cstring>
#include "ametsuchi/newstorage/keyvalue_db_backend.hpp"
//#include "logger/logger.hpp"

namespace iroha {
  namespace newstorage {

    namespace {

      void throwIfLeveldbError(const std::string& db_path, const leveldb::Status& status) {
        if (!status.ok()) {
          throw std::runtime_error(std::string("LevelDB (") + db_path + ") error, " + status.ToString());
        }
      }

      inline leveldb::Slice slice(KeyValueDbBackend::Blob b) {
        return leveldb::Slice((const char*)b.data, b.size);
      }

      inline KeyValueDbBackend::Blob blob(leveldb::Slice s) {
        return KeyValueDbBackend::Blob{s.data(), s.size()};
      }

      inline KeyValueDbBackend::Blob combine(char* buffer, size_t buf_size,
          const std::string& key_scope, char delim, const std::string& key)
      {
        size_t sz = key_scope.size();
        memcpy(buffer, key_scope.c_str(), sz);
        buffer[sz] = delim;
        memcpy(buffer + sz + 1, key.c_str(), key.size());
        return KeyValueDbBackend::Blob{buffer, buf_size};
      }

      inline bool split(const KeyValueDbBackend::Blob& combined, char delim,
          KeyValueDbBackend::Blob& k1, KeyValueDbBackend::Blob& k2)
      {
        if (combined.size == 0) return false;
        const char* p = (const char*)memchr(combined.data, delim, combined.size);
        if (!p) return false;
        k1.data = combined.data;
        k1.size = p - (const char*)combined.data;
        k2.data = p + 1;
        k2.size = combined.size - 1 - k1.size;
        return true;
      }

    } // namespace

    KeyValueDbBackend::KeyValueDbBackend(KeyValueDbBackend::Options options, logger::LoggerPtr log) :
      options_(std::move(options)),
      log_(std::move(log))
    {
      leveldb::DB* db_ptr;
      leveldb::Options o;
      o.create_if_missing = true;
      leveldb::Status status = leveldb::DB::Open(o, options.path, &db_ptr);
      throwIfLeveldbError(options_.path, status);
      db_.reset(db_ptr);
    }

    void KeyValueDbBackend::put(KeyValueDbBackend::Blob key, KeyValueDbBackend::Blob value) {
      leveldb::Status status = db_->Put(leveldb::WriteOptions{}, slice(key), slice(value));
      throwIfLeveldbError(options_.path, status);
    }

    void KeyValueDbBackend::put(const std::string& key, KeyValueDbBackend::Blob value) {
      put(Blob{ key.c_str(), key.size()}, value);
    }

    void KeyValueDbBackend::put(const std::string& key_scope, const std::string& key, KeyValueDbBackend::Blob value) {
      size_t buf_size = key_scope.size() + 1 + key.size();
      char* buffer = (char*)alloca(buf_size);
      put(combine(buffer, buf_size, key_scope, options_.delimiter, key), value);
    }

    std::string KeyValueDbBackend::get(KeyValueDbBackend::Blob key) {
      std::string value;
      leveldb::Status status = db_->Get(leveldb::ReadOptions{}, slice(key), &value);
      if (!status.IsNotFound()) throwIfLeveldbError(options_.path, status);
      return value;
    }

    std::string KeyValueDbBackend::get(const std::string& key) {
      return get(KeyValueDbBackend::Blob{key.c_str(), key.size()});
    }

    std::string KeyValueDbBackend::get(const std::string& key_scope, const std::string& key) {
      size_t buf_size = key_scope.size() + 1 + key.size();
      char* buffer = (char*)alloca(buf_size);
      return get(combine(buffer, buf_size, key_scope, options_.delimiter, key));
    }

    void KeyValueDbBackend::iterate_from(
        const std::string& key,
        std::function<bool(const Blob& key, const Blob& value)> fn)
    {
      std::unique_ptr<leveldb::Iterator> it(db_->NewIterator(leveldb::ReadOptions{}));
      if (key.empty()) {
        it->SeekToFirst();
      } else {
        it->Seek(key);
      }
      leveldb::Status status = it->status();
      if (status.IsNotFound()) {
        return;
      } else {
        throwIfLeveldbError(options_.path, status);
      }
      for (; it->Valid(); it->Next()) {
        bool proceed = fn(blob(it->key()), blob(it->value()));
        if (!proceed) {
          return;
        }
      }
      throwIfLeveldbError(options_.path, it->status());
    }

    void KeyValueDbBackend::iterate_scope(
        const std::string& key,
        std::function<bool(const Blob& key_scope, const Blob& key, const Blob& value)> fn)
    {
      iterate_from(
        key,
        [&key, &fn, this](const Blob& k, const Blob& v) -> bool {
          Blob k1, k2;
          return (
            split(k, options_.delimiter, k1, k2) &&
            k1.size == key.size() &&
            !memcmp(k1.data, key.c_str(), k1.size) &&
            fn(k1, k2, v)
          );
        }
      );
    }

  }  // namespace newstorage
}  // namespace iroha


