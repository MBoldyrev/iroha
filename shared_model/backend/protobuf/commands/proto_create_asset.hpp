/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_PROTO_CREATE_ASSET_HPP
#define IROHA_PROTO_CREATE_ASSET_HPP

#include "interfaces/commands/create_asset.hpp"

#include "commands.pb.h"

namespace shared_model {

  class CreateAsset final : public CreateAsset {
   public:
    explicit CreateAsset(iroha::protocol::Command &command);

    const types::AssetNameType &assetName() const override;

    const types::DomainIdType &domainId() const override;

    const PrecisionType &precision() const override;

   private:
    const iroha::protocol::CreateAsset &create_asset_;

    const PrecisionType precision_;
  };

}  // namespace shared_model

#endif  // IROHA_PROTO_CREATE_ASSET_HPP
