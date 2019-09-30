/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ametsuchi/newstorage/block_query_impl.hpp"
#include "ametsuchi/newstorage/block_index_db.hpp"
#include <boost/format.hpp>
#include "common/byteutils.hpp"
#include "common/cloneable.hpp"
#include "logger/logger.hpp"

namespace iroha {
  namespace newstorage {
    BlockQueryImpl::BlockQueryImpl(BlockIndexDB &db,
        ametsuchi::BlockStorage &block_storage,
        logger::LoggerPtr log)
        : db_(db), block_storage_(block_storage), log_(std::move(log)) {}

    ametsuchi::BlockQuery::BlockResult BlockQueryImpl::getBlock(
        shared_model::interface::types::HeightType height) {
      auto block = block_storage_.fetch(height);
      if (not block) {
        auto error =
            boost::format("Failed to retrieve block with height %d") % height;
        return expected::makeError(
            GetBlockError{GetBlockError::Code::kNoBlock, error.str()});
      }
      return clone(**block);
    }

    shared_model::interface::types::HeightType
    BlockQueryImpl::getTopBlockHeight() {
      return block_storage_.size();
    }

    boost::optional<ametsuchi::TxCacheStatusType> BlockQueryImpl::checkTxPresence(
        const shared_model::crypto::Hash &hash) {

      using namespace ametsuchi;

      int res = -1;
      const auto &hash_str = hash.hex();
      if (!db_.getTxStatusByHash(hash_str, res)) {
        log_->error("Failed to execute query: {}", db_.getLastError());
        return boost::none;
      }

      // res > 0 => Committed
      // res == 0 => Rejected
      // res < 0 => Missing
      if (res > 0) {
        return boost::make_optional<TxCacheStatusType>(
            tx_cache_status_responses::Committed{hash});
      } else if (res == 0) {
        return boost::make_optional<TxCacheStatusType>(
            tx_cache_status_responses::Rejected{hash});
      }
      return boost::make_optional<TxCacheStatusType>(
          tx_cache_status_responses::Missing{hash});
    }

  }  // namespace ametsuchi
}  // namespace iroha
