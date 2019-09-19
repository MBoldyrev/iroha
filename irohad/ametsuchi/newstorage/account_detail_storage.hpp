/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_ACCOUNT_DETAIL_STORAGE_HPP
#define IROHA_ACCOUNT_DETAIL_STORAGE_HPP

#include "ametsuchi/newstorage/keyvalue_db_backend.hpp"
#include "ametsuchi/newstorage/types.hpp"
#include "logger/logger_fwd.hpp"

namespace iroha {
  namespace newstorage {
    class SqliteWrapper;

    class AccountDetailStorage {
     public:
      using GetFunction = std::function<
          bool(
              const Slice& account,
              const Slice& key,
              const Slice& writer,
              const Slice& value
          )
      >;

      AccountDetailStorage(const std::string& path, logger::LoggerPtr log);
      const std::string& getLastError() const { return last_error_; }
      bool put(
          const AccountID& account,
          const AccountID& key,
          const AccountID& writer,
          const Slice& value
      );
      std::string get(
          const AccountID& account,
          const AccountID& writer,
          const ID& key
      );
      bool get(
          const ID& iterate_from,
          const AccountID& account,
          const ID& key,
          const AccountID& writer,
          const GetFunction& callback,
          size_t& total_rows
      );
      bool getByAccountAndKey(
          const ID& iterate_from,
          const AccountID& account,
          const ID& key,
          const GetFunction& callback,
          size_t& total_rows
      );
      bool getByAccountAndWriter(
          const ID& iterate_from,
          const AccountID& account,
          const AccountID& writer,
          const GetFunction& callback,
          size_t& total_rows
      );
      bool getByWriter(
          const ID& iterate_from,
          const AccountID& writer,
          const GetFunction& callback,
          size_t& total_rows
      );
      bool getByAccount(
          const ID& iterate_from,
          const AccountID& account,
          const GetFunction& callback,
          size_t& total_rows
      );

      ID makeCompositeKey(
          const AccountID& account, const ID& key, const AccountID& writer
      );

     private:
      size_t makeKeys(
          const AccountID& account, const ID& key, const AccountID& writer
      );

      size_t makeKey(
          const AccountID& account, const ID& key, const AccountID& writer
      );

      //void makeKey(
      //    const ID& id1, const ID& id2, const ID& id3
      //);

      enum Buffers {
        akw_buffer,
        wa_buffer,
        w_buffer,

        buffers_count
      };

      using StatementHandle = size_t;

      KeyValueDbBackend storage_;
      std::shared_ptr<SqliteWrapper> index_;
      StatementHandle get_n_by_writer_;
      StatementHandle get_n_by_account_;
      StatementHandle get_n_by_writer_account_;
      StatementHandle get_n_by_account_key_;
      StatementHandle inc_writer_;
      StatementHandle inc_account_;
      StatementHandle inc_writer_account_;
      StatementHandle inc_account_key_;
      logger::LoggerPtr log_;
      std::string last_error_;
      std::string buffers_[buffers_count];
    };
  }
}

#endif  // IROHA_ACCOUNT_DETAIL_STORAGE_HPP
