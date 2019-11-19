/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_ACCOUNT_ASSET_RESPONSE_HPP
#define IROHA_SHARED_MODEL_ACCOUNT_ASSET_RESPONSE_HPP

#include "interfaces/base/model_primitive.hpp"

#include <boost/optional.hpp>
#include "interfaces/common_objects/account_asset.hpp"
#include "interfaces/common_objects/range_types.hpp"

namespace shared_model {
  /**
   * Provide response with account asset
   */
  class AccountAssetResponse : public ModelPrimitive<AccountAssetResponse> {
   public:
    /**
     * @return Account has Asset model
     */
    const types::AccountAssetCollectionType accountAssets() const;

    boost::optional<types::AssetIdType> nextAssetId() const;

    size_t totalAccountAssetsNumber() const;

    std::string toString() const override;

    bool operator==(const ModelType &rhs) const override;
  };
}  // namespace shared_model
#endif  // IROHA_SHARED_MODEL_ACCOUNT_ASSET_RESPONSE_HPP
