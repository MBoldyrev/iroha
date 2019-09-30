/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_BLOCK_INDEX_IMPL_HPP
#define IROHA_BLOCK_INDEX_IMPL_HPP

#include "ametsuchi/newstorage/block_index.hpp"

#include "ametsuchi/indexer.hpp"
#include "interfaces/transaction.hpp"
#include "logger/logger_fwd.hpp"

namespace iroha {
  namespace newstorage {
    /**
     * Creates several indices for passed blocks. Namely:
     * transaction hash -> block, where this transaction is stored
     * transaction creator -> block where his transaction is located
     *
     * Additionally, for each Transfer Asset command:
     *   1. (account, asset) -> block for each:
     *     a. creator of the transaction
     *     b. source account
     *     c. destination account
     *   2. account -> block for source and destination accounts
     *   3. (account, height) -> list of txes
     */
    class BlockIndexImpl : public BlockIndex {
     public:
      BlockIndexImpl(std::unique_ptr<ametsuchi::Indexer> indexer,
                         logger::LoggerPtr log);

      /// Index a block.
      void index(const shared_model::interface::Block &block) override;

     private:
      /// Index a transaction.
      void makeAccountAssetIndex(
          const shared_model::interface::types::AccountIdType &account_id,
          ametsuchi::Indexer::TxPosition position,
          const shared_model::interface::Transaction::CommandsType &commands);

      std::unique_ptr<ametsuchi::Indexer> indexer_;
      logger::LoggerPtr log_;
    };
  }  // namespace ametsuchi
}  // namespace iroha

#endif  // IROHA_BLOCK_INDEX_IMPL_HPP
