/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_BLOCK_QUERY_IMPL_HPP
#define IROHA_BLOCK_QUERY_IMPL_HPP

#include "ametsuchi/block_query.hpp"
#include <boost/optional.hpp>
#include "ametsuchi/block_storage.hpp"
#include "logger/logger_fwd.hpp"

namespace iroha {
  namespace newstorage {

    class BlockIndexDB;

    /**
     * Class which implements BlockQuery
     */
   class BlockQueryImpl : public ametsuchi::BlockQuery {
     public:
      BlockQueryImpl(BlockIndexDB &db,
                     ametsuchi::BlockStorage &block_storage,
                         logger::LoggerPtr log);

      BlockResult getBlock(
          shared_model::interface::types::HeightType height) override;

      shared_model::interface::types::HeightType getTopBlockHeight() override;

      boost::optional<ametsuchi::TxCacheStatusType> checkTxPresence(
          const shared_model::crypto::Hash &hash) override;

     private:
      BlockIndexDB& db_;
      ametsuchi::BlockStorage &block_storage_;
      logger::LoggerPtr log_;
    };
  }  // namespace newstorage
}  // namespace iroha

#endif  // IROHA_BLOCK_QUERY_IMPL_HPP
