/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "backend/protobuf/commands/proto_transfer_asset.hpp"

TransferAsset::TransferAsset(iroha::protocol::Command &command)
    : transfer_asset_{command.transfer_asset()},
      amount_{transfer_asset_.amount()} {}

const Amount &TransferAsset::amount() const {
  return amount_;
}

const types::AssetIdType &TransferAsset::assetId() const {
  return transfer_asset_.asset_id();
}

const types::AccountIdType &TransferAsset::srcAccountId() const {
  return transfer_asset_.src_account_id();
}

const types::AccountIdType &TransferAsset::destAccountId() const {
  return transfer_asset_.dest_account_id();
}

const types::DescriptionType &TransferAsset::description() const {
  return transfer_asset_.description();
}
