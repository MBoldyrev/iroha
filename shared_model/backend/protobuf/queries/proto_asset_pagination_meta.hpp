/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_PROTO_MODEL_QUERY_ACCOUNT_ASSET_PAGINATION_META_HPP
#define IROHA_SHARED_PROTO_MODEL_QUERY_ACCOUNT_ASSET_PAGINATION_META_HPP

#include "interfaces/queries/asset_pagination_meta.hpp"

#include "interfaces/common_objects/types.hpp"
#include "queries.pb.h"

namespace shared_model {

  /// Provides query metadata for AccountAsset list pagination.
  class AssetPaginationMeta final : public AssetPaginationMeta {
   public:
    explicit AssetPaginationMeta(iroha::protocol::AssetPaginationMeta &meta);

    types::TransactionsNumberType pageSize() const override;

    boost::optional<types::AssetIdType> firstAssetId() const override;

   private:
    const iroha::protocol::AssetPaginationMeta &meta_;
  };
}  // namespace shared_model

#endif  // IROHA_SHARED_PROTO_MODEL_QUERY_ACCOUNT_ASSET_PAGINATION_META_HPP
