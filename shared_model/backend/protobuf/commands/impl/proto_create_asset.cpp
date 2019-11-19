/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "backend/protobuf/commands/proto_create_asset.hpp"

CreateAsset::CreateAsset(iroha::protocol::Command &command)
    : create_asset_{command.create_asset()},
      precision_{static_cast<const PrecisionType>(create_asset_.precision())} {}

const types::AssetNameType &CreateAsset::assetName() const {
  return create_asset_.asset_name();
}

const types::DomainIdType &CreateAsset::domainId() const {
  return create_asset_.domain_id();
}

const CreateAsset::PrecisionType &CreateAsset::precision() const {
  return precision_;
}
