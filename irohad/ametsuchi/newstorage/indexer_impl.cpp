/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ametsuchi/newstorage/indexer_impl.hpp"
#include "ametsuchi/newstorage/block_index_db.hpp"
#include "cryptography/hash.hpp"

namespace iroha { namespace newstorage {
  using namespace shared_model::interface::types;

  IndexerImpl::IndexerImpl(std::shared_ptr<BlockIndexDB> db)
      : db_(std::move(db)) {}

  void IndexerImpl::begin() {
    if (!inside_tx_) {
      db_->beginTx();
      inside_tx_ = true;
    }
  }

  void IndexerImpl::txHashPosition(const HashType &hash, TxPosition position) {
    if (!error_state_) {
      begin();
      error_state_ =
          db_->insertPositionByHash(hash.hex(), position.height, position.index)
          != 1;
    }
  }

  void IndexerImpl::txHashStatus(const HashType &hash, int status) {
    if (!error_state_) {
      begin();
      error_state_ = db_->insertStatusByHash(hash.hex(), status) != 1;
    }
  }

  void IndexerImpl::committedTxHash(const HashType &committed_tx_hash) {
    txHashStatus(committed_tx_hash, 1);
  }

  void IndexerImpl::rejectedTxHash(const HashType &rejected_tx_hash) {
    txHashStatus(rejected_tx_hash, 0);
  }

  void IndexerImpl::txPositionByCreator(const AccountIdType creator,
                                        TxPosition position) {
    if (!error_state_) {
      begin();
      error_state_ =
          db_->insertPositionByCreator(creator, position.height, position.index)
          != 1;
    }
  }

  void IndexerImpl::accountAssetTxPosition(const AccountIdType &account_id,
                                           const AssetIdType &asset_id,
                                           TxPosition position) {
    if (!error_state_) {
      begin();
      error_state_ = db_->insertPositionByAccountAsset(
                         account_id, asset_id, position.height, position.index)
          != 1;
    }
  }

  iroha::expected::Result<void, std::string> IndexerImpl::flush() {
    if (!inside_tx_) {
      return {};
    }
    inside_tx_ = false;
    if (error_state_) {
      auto err_msg = db_->getLastError();
      db_->rollbackTx();
      error_state_ = false;
      return err_msg;
    }
    db_->commitTx();
    return {};
  }
}} //namespace
