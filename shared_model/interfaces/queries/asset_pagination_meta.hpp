/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_INTERFACE_MODEL_QUERY_ACCOUNT_ASSET_PAGINATION_META_HPP
#define IROHA_SHARED_INTERFACE_MODEL_QUERY_ACCOUNT_ASSET_PAGINATION_META_HPP

#include <boost/optional.hpp>

#include "interfaces/base/model_primitive.hpp"
#include "interfaces/common_objects/types.hpp"
#include "queries.pb.h"

namespace shared_model {

  /// Provides query metadata for asset list pagination.
  class AssetPaginationMeta : public ModelPrimitive<AssetPaginationMeta> {
   public:
    explicit AssetPaginationMeta(iroha::protocol::AssetPaginationMeta &meta);

    /// Get the requested page size.
    types::TransactionsNumberType pageSize() const;

    /// Get the first requested asset, if provided.
    boost::optional<types::AssetIdType> firstAssetId() const;

    std::string toString() const override;

    bool operator==(const ModelType &rhs) const override;

   private:
    const iroha::protocol::AssetPaginationMeta &meta_;
  };

}  // namespace shared_model

#endif  // IROHA_SHARED_INTERFACE_MODEL_QUERY_ACCOUNT_ASSET_PAGINATION_META_HPP
