/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "backend/protobuf/queries/proto_get_account_assets.hpp"

GetAccountAssets::GetAccountAssets(iroha::protocol::Query &query)
    : account_assets_{query.payload().get_account_assets()},
      pagination_meta_{[&]() -> boost::optional<const AssetPaginationMeta> {
        if (query.payload().get_account_assets().has_pagination_meta()) {
          return AssetPaginationMeta{*query.mutable_payload()
                                          ->mutable_get_account_assets()
                                          ->mutable_pagination_meta()};
        } else {
          return boost::none;
        }
      }()} {}

const interface::types::AccountIdType &GetAccountAssets::accountId() const {
  return account_assets_.account_id();
}

boost::optional<const interface::AssetPaginationMeta &>
GetAccountAssets::paginationMeta() const {
  if (pagination_meta_) {
    return pagination_meta_.value();
  }
  return boost::none;
}
