/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_PROTO_GET_ASSET_INFO_H
#define IROHA_PROTO_GET_ASSET_INFO_H

#include "interfaces/queries/get_asset_info.hpp"

#include "queries.pb.h"

namespace shared_model {
  class GetAssetInfo final : public GetAssetInfo {
   public:
    explicit GetAssetInfo(iroha::protocol::Query &query);

    const types::AssetIdType &assetId() const override;

   private:
    // ------------------------------| fields |-------------------------------
    const iroha::protocol::GetAssetInfo &asset_info_;
  };

}  // namespace shared_model

#endif  // IROHA_PROTO_GET_ASSET_INFO_H
