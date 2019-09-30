/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AMETSUCHI_INDEXER_IMPL_HPP
#define AMETSUCHI_INDEXER_IMPL_HPP

#include "ametsuchi/indexer.hpp"

namespace iroha {
  namespace newstorage {
   class BlockIndexDB;

   class IndexerImpl : public ametsuchi::Indexer {
     public:
      explicit IndexerImpl(std::shared_ptr<BlockIndexDB> db);

      void txHashPosition(const shared_model::interface::types::HashType &hash,
                          TxPosition position) override;

      void committedTxHash(const shared_model::interface::types::HashType
                               &committed_tx_hash) override;

      void rejectedTxHash(const shared_model::interface::types::HashType
                              &rejected_tx_hash) override;

      void txPositionByCreator(
          const shared_model::interface::types::AccountIdType creator,
          TxPosition position) override;

      void accountAssetTxPosition(
          const shared_model::interface::types::AccountIdType &account_id,
          const shared_model::interface::types::AssetIdType &asset_id,
          TxPosition position) override;

      iroha::expected::Result<void, std::string> flush() override;

     private:
      /// Index tx status by its hash.
      void txHashStatus(
          const shared_model::interface::types::HashType &hash,
          int status);

      void begin();

      std::shared_ptr<BlockIndexDB> db_;
      bool inside_tx_ = false;
      bool error_state_ = false;
    };

  }  // namespace ametsuchi
}  // namespace iroha

#endif /* AMETSUCHI_INDEXER_IMPL_HPP */
