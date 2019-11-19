/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_ASSET_HPP
#define IROHA_SHARED_MODEL_ASSET_HPP

#include "interfaces/base/model_primitive.hpp"
#include "interfaces/common_objects/types.hpp"
#include "utils/string_builder.hpp"

namespace shared_model {

  /**
   * Representation of valuable goods in the system
   */
  class Asset : public ModelPrimitive<Asset> {
   public:
    /**
     * @return Identity of asset
     */
    const types::AccountIdType &assetId() const;

    /**
     * @return Identity of domain
     */
    const types::DomainIdType &domainId() const;

    /**
     * @return Asset's fixed precision
     */
    types::PrecisionType precision() const;

    /**
     * Stringify the data.
     * @return the content of asset.
     */
    std::string toString() const override {
      return detail::PrettyStringBuilder()
          .init("Asset")
          .append("assetId", assetId())
          .append("domainId", domainId())
          .append("precision", std::to_string(precision()))
          .finalize();
    }

    /**
     * Checks equality of objects inside
     * @param rhs - other wrapped value
     * @return true, if wrapped objects are same
     */
    bool operator==(const ModelType &rhs) const override {
      return assetId() == rhs.assetId() and domainId() == rhs.domainId()
          and precision() == rhs.precision();
    }
  };
}  // namespace shared_model
#endif  // IROHA_SHARED_MODEL_ASSET_HPP
