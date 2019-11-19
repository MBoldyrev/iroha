/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_GET_ACCOUNT_ASSET_TRANSACTIONS_HPP
#define IROHA_SHARED_MODEL_GET_ACCOUNT_ASSET_TRANSACTIONS_HPP

#include <memory>

#include "backend/protobuf/queries/proto_tx_pagination_meta.hpp"
#include "queries.pb.h"

#include "interfaces/base/model_primitive.hpp"
#include "interfaces/common_objects/types.hpp"

namespace shared_model {
  class TxPaginationMeta;

  /**
   * Query for getting transactions of given asset of an account
   */
  class GetAccountAssetTransactions
      : public ModelPrimitive<GetAccountAssetTransactions> {
   public:
    explicit GetAccountAssetTransactions(iroha::protocol::Query &query);

    /**
     * @return account_id of requested transactions
     */
    const types::AccountIdType &accountId() const;
    /**
     * @return assetId of requested transactions
     */
    const types::AccountIdType &assetId() const;

    /// Get the query pagination metadata.
    const TxPaginationMeta &paginationMeta() const;

    std::string toString() const override;

    bool operator==(const ModelType &rhs) const override;

   private:
    // ------------------------------| fields |-------------------------------

    const iroha::protocol::GetAccountAssetTransactions
        &account_asset_transactions_;
    const TxPaginationMeta pagination_meta_;
  };

}  // namespace shared_model

#endif  // IROHA_SHARED_MODEL_GET_ACCOUNT_ASSET_TRANSACTIONS_HPP
