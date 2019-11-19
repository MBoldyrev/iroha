/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_CREATE_ASSET_HPP
#define IROHA_SHARED_MODEL_CREATE_ASSET_HPP

#include "interfaces/base/model_primitive.hpp"

#include "commands.pb.h"

#include "interfaces/common_objects/types.hpp"

namespace shared_model {
  /**
   * Create asset in Iroha domain
   */
  class CreateAsset : public ModelPrimitive<CreateAsset> {
   public:
    explicit CreateAsset(iroha::protocol::Command &command);

    /**
     * @return Asset name to create
     */
    const types::AssetNameType &assetName() const;
    /**
     * @return Iroha domain of the asset
     */
    const types::DomainIdType &domainId() const;
    /// Precision type
    using PrecisionType = uint8_t;
    /**
     * @return precision of the asset
     */
    const PrecisionType &precision() const;

    std::string toString() const override;

    bool operator==(const ModelType &rhs) const override;

   private:
    const iroha::protocol::CreateAsset &create_asset_;

    const PrecisionType precision_;
  };
}  // namespace shared_model

#endif  // IROHA_SHARED_MODEL_CREATE_ASSET_HPP
