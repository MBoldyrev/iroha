/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_GET_ACCOUNT_ASSETS_HPP
#define IROHA_SHARED_MODEL_GET_ACCOUNT_ASSETS_HPP

#include <boost/optional.hpp>

#include <boost/optional.hpp>
#include "backend/protobuf/queries/proto_asset_pagination_meta.hpp"
#include "interfaces/base/model_primitive.hpp"
#include "interfaces/common_objects/types.hpp"
#include "queries.pb.h"

namespace shared_model {
  class AssetPaginationMeta;

  /**
   * Query for get all account's assets and balance
   */
  class GetAccountAssets : public ModelPrimitive<GetAccountAssets> {
   public:
    explicit GetAccountAssets(iroha::protocol::Query &query);

    /**
     * @return account identifier
     */
    const types::AccountIdType &accountId() const;

    /// Get the query pagination metadata.
    // TODO 2019.05.24 mboldyrev IR-516 remove optional
    boost::optional<const AssetPaginationMeta &> paginationMeta() const;

    std::string toString() const override;

    bool operator==(const ModelType &rhs) const override;

   private:
    // ------------------------------| fields |-------------------------------

    const iroha::protocol::GetAccountAssets &account_assets_;
    const boost::optional<const AssetPaginationMeta> pagination_meta_;
  };
}  // namespace shared_model

#endif  // IROHA_SHARED_MODEL_GET_ACCOUNT_ASSETS_HPP
