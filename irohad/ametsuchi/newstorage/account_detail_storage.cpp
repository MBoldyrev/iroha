/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ametsuchi/newstorage/account_detail_storage.hpp"
#include "ametsuchi/newstorage/sqlite_wrapper.hpp"
#include <cstdio>

namespace iroha {
  namespace newstorage {
    namespace {
      const char kDELIMITER = 0x01; ///TODO ??
      const char kWRITER_PREFIX = 0x02;

      const char* kWRITER = "writer";
      const char* kACCOUNT = "account";
      const char* kWRITERACCOUNT = "writeraccount";
      const char* kACCOUNTKEY = "accountkey";

      const char* create_statement_fmt =
          "CREATE TABLE IF NOT EXISTS by%1$s "
          "(%1$s TEXT PRIMARY KEY, n INTEGER NOT NULL)";
      const char* get_statement_fmt =
          "SELECT n FROM by%1$s WHERE %1$s=?";
      const char* inc_statement_fmt =
          "INSERT OR REPLACE INTO by%1$s VALUES "
          "(?, COALESCE((SELECT n FROM by%1$s WHERE %1$s=?), 0) + 1)";

      inline bool split_akw(const Slice& akw, Slice& account, Slice& key, Slice& writer) {
        auto start = akw.data();
        auto end = start + akw.size();
        auto p = (const char*)memchr(start, kDELIMITER, end - start);
        if (!p) return false;
        account = Slice(start, p - start);
        start = ++p;
        p = (const char*)memchr(start, kDELIMITER, end - start);
        if (!p) return false;
        key = Slice(start, p - start);
        ++p;
        writer = Slice(p, end - p);
        return true;
      }

    } //namespace

    AccountDetailStorage::AccountDetailStorage(
        const std::string& path,
        logger::LoggerPtr log)
      : log_(std::move(log))
    {
      KeyValueDbBackend::Options kv_options;
      kv_options.path = path;
      storage_.create(std::move(kv_options));
      SqliteWrapper::Options sql_options;
      sql_options.db_file = path + "/index.db";
      sql_options.log_prefix = "Account detail DB: ";
      index_ = SqliteWrapper::create(sql_options, log_);

      static const size_t BUF_SIZE = 1024;
      char buf[BUF_SIZE];

      snprintf(buf, BUF_SIZE, create_statement_fmt, kWRITER);
      *index_ << buf;
      snprintf(buf, BUF_SIZE, create_statement_fmt, kACCOUNT);
      *index_ << buf;
      snprintf(buf, BUF_SIZE, create_statement_fmt, kWRITERACCOUNT);
      *index_ << buf;
      snprintf(buf, BUF_SIZE, create_statement_fmt, kACCOUNTKEY);
      *index_ << buf;

      snprintf(buf, BUF_SIZE, get_statement_fmt, kWRITER);
      get_n_by_writer_ = index_->createStatement(buf);
      snprintf(buf, BUF_SIZE, get_statement_fmt, kACCOUNT);
      get_n_by_account_ = index_->createStatement(buf);
      snprintf(buf, BUF_SIZE, get_statement_fmt, kWRITERACCOUNT);
      get_n_by_writer_account_ = index_->createStatement(buf);
      snprintf(buf, BUF_SIZE, get_statement_fmt, kACCOUNTKEY);
      get_n_by_account_key_ = index_->createStatement(buf);

      snprintf(buf, BUF_SIZE, inc_statement_fmt, kWRITER);
      inc_writer_ = index_->createStatement(buf);
      snprintf(buf, BUF_SIZE, inc_statement_fmt, kACCOUNT);
      inc_account_ = index_->createStatement(buf);
      snprintf(buf, BUF_SIZE, inc_statement_fmt, kWRITERACCOUNT);
      inc_writer_account_ = index_->createStatement(buf);
      snprintf(buf, BUF_SIZE, inc_statement_fmt, kACCOUNTKEY);
      inc_account_key_ = index_->createStatement(buf);
    }

    bool AccountDetailStorage::put(
        const AccountID& account,
        const AccountID& key,
        const AccountID& writer,
        const Slice& value
    ) {
      try {
        size_t ak_bufsize = makeKeys(account, key, writer);

        bool need_to_increment = not storage_.keyExists(Slice(buffers_[0]));

        {
          KeyValueDbBackend::WriteBatch wb;
          wb.put(Slice(buffers_[akw_buffer]), value);
          wb.put(Slice(buffers_[w_buffer]),
                 Slice(buffers_[akw_buffer].c_str(), ak_bufsize));
          if (!storage_.apply(wb)) {
            // TODO log
            return false;
          }
        }

        if (need_to_increment) {
          SqliteWrapper::Transaction tx(*index_);
          auto& inc_writer_st = index_->getStatement(inc_writer_);
          inc_writer_st << writer;
          inc_writer_st++;
          auto& inc_account_st = index_->getStatement(inc_account_);
          inc_account_st << account;
          inc_account_st++;
          auto& inc_writer_account_st = index_->getStatement(inc_writer_account_);
          inc_writer_account_st << buffers_[wa_buffer];
          inc_writer_account_st++;
          buffers_[akw_buffer].resize(ak_bufsize);
          auto& inc_account_key_st = index_->getStatement(inc_account_key_);
          inc_account_key_st << buffers_[akw_buffer];
          inc_account_key_st++;
          tx.commit();
        }
      } catch (const std::exception& e) {
        // TODO log
        return false;
      }

      return true;
    }

    std::string AccountDetailStorage::get(
        const AccountID &account,
        const AccountID &writer,
        const ID &key
    ) {
      makeKey(account, key, writer);
      return storage_.get(buffers_[akw_buffer]);
    }

    bool AccountDetailStorage::getByAccountAndKey(
        const ID& iterate_from,
        const AccountID& account,
        const ID& key,
        const GetFunction& callback,
        size_t& total_rows
    ) {
      index_->getStatement(get_n_by_account_key_) >> total_rows;
      if (total_rows == 0) {
        return true;
      }
      storage_.iterate_from(
          Slice(iterate_from),
          [=](const Slice& akw, const Slice& value) -> bool {
            Slice a, k, w;
            if (split_akw(akw, a, k, w)) {
              if (Slice(account) == a && Slice(key) == k) {
                return callback(a, k, w, value);
              }
            }
            return false;
          }
      );
      return true; //TODO try-catch
    }

    bool AccountDetailStorage::getByAccountAndWriter(
        const ID& iterate_from,
        const AccountID& account,
        const AccountID& writer,
        const GetFunction& callback,
        size_t& total_rows
    ) {
      index_->getStatement(get_n_by_writer_account_) >> total_rows;
      if (total_rows == 0) {
        return true;
      }
      storage_.iterate_from(
          Slice(iterate_from),
          [=](const Slice& akw, const Slice& value) -> bool {
            Slice a, k, w;
            if (split_akw(akw, a, k, w)) {
              if (Slice(account) == a && Slice(writer) == w) {
                return callback(a, k, w, value);
              }
            }
            return false;
          }
      );
      return true; //TODO try-catch
    }

    bool AccountDetailStorage::getByWriter(
        const ID& iterate_from,
        const AccountID& writer,
        const GetFunction& callback,
        size_t& total_rows
    ) {
      index_->getStatement(get_n_by_writer_) >> total_rows;
      if (total_rows == 0) {
        return true;
      }
      storage_.iterate_from(
          Slice(iterate_from),
          [=](const Slice& akw, const Slice& value) -> bool {
            Slice a, k, w;
            if (split_akw(akw, a, k, w)) {
              if (Slice(writer) == w) {
                return callback(a, k, w, value);
              }
            }
            return false;
          }
      );
      return true; //TODO try-catch
    }

    bool AccountDetailStorage::getByAccount(
        const ID& iterate_from,
        const AccountID& account,
        const GetFunction& callback,
        size_t& total_rows
    ) {
      index_->getStatement(get_n_by_account_) >> total_rows;
      if (total_rows == 0) {
        return true;
      }
      storage_.iterate_from(
          Slice(iterate_from),
          [=](const Slice& akw, const Slice& value) -> bool {
            Slice a, k, w;
            if (split_akw(akw, a, k, w)) {
              if (Slice(account) == a) {
                return callback(a, k, w, value);
              }
            }
            return false;
          }
      );
      return true; //TODO try-catch
    }

    size_t AccountDetailStorage::makeKeys(
        const AccountID& account, const ID& key, const AccountID& writer
    ) {
      size_t ak_size = makeKey(account, key, writer);

      std::string& wa = buffers_[wa_buffer];
      wa.clear();
      wa.append(writer);
      wa.push_back(kDELIMITER);
      wa.append(account);

      std::string& w = buffers_[w_buffer];
      w.clear();
      w.push_back(kWRITER_PREFIX);
      w.append(writer);

      return ak_size;
    }

    size_t AccountDetailStorage::makeKey(
        const AccountID& account, const ID& key, const AccountID& writer
    ) {
      std::string& akw = buffers_[akw_buffer];
      akw.clear();
      akw.append(account);
      akw.push_back(kDELIMITER);
      akw.append(key);
      size_t ak_size = akw.size();
      akw.push_back(kDELIMITER);
      akw.append(writer);
      return ak_size;
    }

    ID AccountDetailStorage::makeCompositeKey(
        const AccountID& account, const ID& key, const AccountID& writer
    ) {
      makeKey(account, key, writer);
      return ID(buffers_[akw_buffer]);
    }

    bool AccountDetailStorage::get(
        const ID& iterate_from,
        const AccountID& account,
        const ID& key,
        const AccountID& writer,
        const GetFunction& callback,
        size_t& total_rows
    ) {
      if (account.empty()) return false;
      if (key.empty()) {
        if (!writer.empty()) {
          return getByAccountAndWriter(
              iterate_from, account, writer, callback, total_rows);
        }
      }
      if (writer.empty()) {
        if (!key.empty()) {
          return getByAccountAndKey(
              iterate_from, account, key, callback, total_rows);
        }
      }
      return getByAccount(iterate_from, account, callback, total_rows);
    }

  }
}
