/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_ASSET_HPP
#define IROHA_SHARED_MODEL_ASSET_HPP

#include "interfaces/common_objects/trivial_proto.hpp"
#include "interfaces/common_objects/types.hpp"
#include "qry_responses.pb.h"

namespace shared_model {

  /**
   * Representation of valuable goods in the system
   */
  class Asset : public TrivialProto<Asset, iroha::protocol::Asset> {
   public:
    template <typename T>
    explicit Asset(T &&asset) : TrivialProto(std::forward<T>(asset)) {}

    Asset(const types::AssetIdType &asset_id,
          const types::DomainIdType &domain_id,
          types::PrecisionType precision);

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
    std::string toString() const override;

    /**
     * Checks equality of objects inside
     * @param rhs - other wrapped value
     * @return true, if wrapped objects are same
     */
    bool operator==(const ModelType &rhs) const override;
  };
}  // namespace shared_model
#endif  // IROHA_SHARED_MODEL_ASSET_HPP
