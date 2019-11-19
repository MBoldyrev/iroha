/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_TRANSFER_ASSET_HPP
#define IROHA_SHARED_MODEL_TRANSFER_ASSET_HPP

#include "interfaces/base/model_primitive.hpp"

#include "interfaces/common_objects/amount.hpp"
#include "interfaces/common_objects/types.hpp"

namespace shared_model {
  /**
   * Grant permission to account
   */
  class TransferAsset : public ModelPrimitive<TransferAsset> {
   public:
    /**
     * @return Id of the account from which transfer assets
     */
    const types::AccountIdType &srcAccountId() const;
    /**
     * @return Id of the account to which transfer assets
     */
    const types::AccountIdType &destAccountId() const;
    /**
     * @return Id of the asset to transfer
     */
    const types::AssetIdType &assetId() const;
    /**
     * @return asset amount to transfer
     */
    const Amount &amount() const;
    /**
     * @return message of the transfer
     */
    const types::DescriptionType &description() const;

    std::string toString() const override;

    bool operator==(const ModelType &rhs) const override;
  };
}  // namespace shared_model

#endif  // IROHA_SHARED_MODEL_TRANSFER_ASSET_HPP
