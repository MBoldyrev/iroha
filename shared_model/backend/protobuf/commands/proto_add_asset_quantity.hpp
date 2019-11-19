/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_PROTO_ADD_ASSET_QUANTITY_HPP
#define IROHA_PROTO_ADD_ASSET_QUANTITY_HPP

#include "interfaces/commands/add_asset_quantity.hpp"

#include "commands.pb.h"
#include "interfaces/common_objects/amount.hpp"

namespace shared_model {
  class AddAssetQuantity final : public AddAssetQuantity {
   public:
    explicit AddAssetQuantity(iroha::protocol::Command &command);

    const types::AssetIdType &assetId() const override;

    const Amount &amount() const override;

   private:
    const iroha::protocol::AddAssetQuantity &add_asset_quantity_;

    const Amount amount_;
  };
}  // namespace shared_model

#endif  // IROHA_PROTO_ADD_ASSET_QUANTITY_HPP
