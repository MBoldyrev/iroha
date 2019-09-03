/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <leveldb/db.h>
#include <leveldb/env.h>
#include <cstring>
#include <boost/endian/conversion.hpp>
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

      inline Slice combine(char* buffer, size_t buf_size,
          const Slice& key_scope, char delim, const Slice& key)
      {
        size_t sz = key_scope.size();
        memcpy(buffer, key_scope.data(), sz);
        buffer[sz] = delim;
        memcpy(buffer + sz + 1, key.data(), key.size());
        return Slice(buffer, buf_size);
      }

      inline bool split(const Slice& combined, char delim,
          Slice& k1, Slice& k2)
      {
        if (combined.empty()) return false;
        const char* p = (const char*)memchr(combined.data(), delim, combined.size());
        if (!p) return false;
        k1 = Slice(combined.data(), p - combined.data());
        k2 = Slice(p + 1, combined.size() - 1 - k1.size());
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

      // cannot create dirs recursively... support 2 level at the moment
      // TODO: std::filesystem::create_diectories or #ifdef WIN32 '\\'
      o.env->CreateDir(options_.path.substr(0, options_.path.find('/')));

      leveldb::Status status = leveldb::DB::Open(o, options_.path, &db_ptr);
      throwIfLeveldbError(options_.path, status);
      db_.reset(db_ptr);
    }

    void KeyValueDbBackend::put(const Slice& key, const Slice& value) {
      leveldb::Status status = db_->Put(leveldb::WriteOptions{}, key, value);
      throwIfLeveldbError(options_.path, status);
    }

    void KeyValueDbBackend::put(const Slice& key_scope, const Slice& key, const Slice& value) {
      size_t buf_size = key_scope.size() + 1 + key.size();
      char* buffer = (char*)alloca(buf_size);
      put(combine(buffer, buf_size, key_scope, options_.delimiter, key), value);
    }

    void KeyValueDbBackend::put(uint64_t key, const Slice& value) {
      uint64_t k = boost::endian::native_to_big(key);
      put(slice(&k, 8), value);
    }

    std::string KeyValueDbBackend::get(const Slice& key) {
      std::string value;
      leveldb::Status status = db_->Get(leveldb::ReadOptions{}, key, &value);
      if (!status.IsNotFound()) throwIfLeveldbError(options_.path, status);
      return value;
    }

    std::string KeyValueDbBackend::get(const Slice& key_scope, const Slice& key) {
      size_t buf_size = key_scope.size() + 1 + key.size();
      char* buffer = (char*)alloca(buf_size);
      return get(combine(buffer, buf_size, key_scope, options_.delimiter, key));
    }

    std::string KeyValueDbBackend::get(uint64_t key) {
      uint64_t k = boost::endian::native_to_big(key);
      return get(slice(&k, 8));
    }

    void KeyValueDbBackend::iterate_from(
        const Slice& key,
        const std::function<bool(const Slice& key, const Slice& value)>& fn)
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
        bool proceed = fn(it->key(), it->value());
        if (!proceed) {
          return;
        }
      }
      throwIfLeveldbError(options_.path, it->status());
    }

    void KeyValueDbBackend::iterate_from(
        uint64_t key,
        const std::function<bool(uint64_t key, const Slice& value)>& fn)
    {
      uint64_t kn = boost::endian::native_to_big(key);
      iterate_from(
          slice(&kn, 8),
          [&fn](const Slice& k, const Slice& v) -> bool {
            if (k.size() != 8) return false;
            uint64_t x;
            memcpy(&x, k.data(), 8);
            boost::endian::big_to_native_inplace(x);
            return fn(x, v);
          }
      );
    }

    void KeyValueDbBackend::iterate_scope(
        const Slice& key_scope,
        const std::function<bool(const Slice& key_scope, const Slice& key, const Slice& value)>& fn)
    {
      iterate_from(
        key_scope,
        [&key_scope, &fn, this](const Slice& k, const Slice& v) -> bool {
          Slice k1, k2;
          return (
            split(k, options_.delimiter, k1, k2) &&
            k1 == key_scope &&
            fn(k1, k2, v)
          );
        }
      );
    }

  }  // namespace newstorage
}  // namespace iroha


