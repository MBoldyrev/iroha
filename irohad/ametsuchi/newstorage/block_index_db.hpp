/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_BLOCK_INDEX_DB_HPP
#define IROHA_BLOCK_INDEX_DB_HPP

#include <functional>
#include <string>
#include <vector>
#include "logger/logger_fwd.hpp"

namespace iroha {
  namespace newstorage {
    class SqliteWrapper;
    using StatementHandle = size_t;

    class BlockIndexDB {
     public:
      explicit BlockIndexDB(std::shared_ptr<SqliteWrapper> db);

      std::string getLastError();

      void beginTx();
      void commitTx();
      void rollbackTx();

      // query functions dont throw

      bool getLedgerState(uint64_t& height, std::string& hash);

      bool getTxStatusByHash(const std::string& hash, int& status);

      // modifying requests return rows affected or -1 on error

      int setLedgerState(uint64_t height, const std::string& hash);

      int insertPositionByHash(const std::string& hash, uint64_t height, size_t idx);

      int insertStatusByHash(const std::string& hash, int status);

      int insertPositionByCreator(const std::string& creator_id,
                                  uint64_t height, size_t idx);

      int insertPositionByAccountAsset(const std::string& account_id,
          const std::string& asset_id, uint64_t height, size_t idx);

     private:
      void createSchema();

      std::shared_ptr<SqliteWrapper> db_;
      StatementHandle get_ledger_state_ = 0;
      StatementHandle set_ledger_state_ = 0;
      StatementHandle insert_position_by_hash_ = 0;
      StatementHandle insert_status_by_hash_ = 0;
      StatementHandle insert_position_by_creator_ = 0;
      StatementHandle insert_position_by_account_asset_ = 0;
    };
  }  // namespace newstorage
}  // namespace iroha

#endif  // IROHA_BLOCK_INDEX_DB_HPP
