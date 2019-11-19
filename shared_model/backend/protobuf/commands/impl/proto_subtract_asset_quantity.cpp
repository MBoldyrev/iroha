/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "backend/protobuf/commands/proto_subtract_asset_quantity.hpp"

SubtractAssetQuantity::SubtractAssetQuantity(iroha::protocol::Command &command)
    : subtract_asset_quantity_{command.subtract_asset_quantity()},
      amount_{subtract_asset_quantity_.amount()} {}

const types::AssetIdType &SubtractAssetQuantity::assetId() const {
  return subtract_asset_quantity_.asset_id();
}

const Amount &SubtractAssetQuantity::amount() const {
  return amount_;
}
