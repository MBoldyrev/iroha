/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_ADD_ASSET_QUANTITY_HPP
#define IROHA_SHARED_MODEL_ADD_ASSET_QUANTITY_HPP

#include "interfaces/base/model_primitive.hpp"

#include "commands.pb.h"
#include "interfaces/common_objects/amount.hpp"

#include "interfaces/common_objects/amount.hpp"
#include "interfaces/common_objects/types.hpp"

namespace shared_model {

  /**
   * Add amount of asset to an account
   */
  class AddAssetQuantity : public ModelPrimitive<AddAssetQuantity> {
   public:
    explicit AddAssetQuantity(iroha::protocol::Command &command);

    /**
     * @return asset identifier
     */
    const types::AssetIdType &assetId() const;
    /**
     * @return quantity of asset for adding
     */
    const Amount &amount() const;

    std::string toString() const override;

    bool operator==(const ModelType &rhs) const override;

   private:
    const iroha::protocol::AddAssetQuantity &add_asset_quantity_;

    const Amount amount_;
  };
}  // namespace shared_model
#endif  // IROHA_SHARED_MODEL_ADD_ASSET_QUANTITY_HPP
