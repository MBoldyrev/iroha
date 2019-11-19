/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_ADD_ASSET_QUANTITY_HPP
#define IROHA_SHARED_MODEL_ADD_ASSET_QUANTITY_HPP

#include "interfaces/base/model_primitive.hpp"

#include "interfaces/common_objects/amount.hpp"
#include "interfaces/common_objects/types.hpp"

namespace shared_model {

  /**
   * Add amount of asset to an account
   */
  class AddAssetQuantity : public ModelPrimitive<AddAssetQuantity> {
   public:
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
  };
}  // namespace shared_model
#endif  // IROHA_SHARED_MODEL_ADD_ASSET_QUANTITY_HPP
