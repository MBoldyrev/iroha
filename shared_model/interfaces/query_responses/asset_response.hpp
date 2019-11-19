/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_ASSET_RESPONSE_HPP
#define IROHA_SHARED_MODEL_ASSET_RESPONSE_HPP

#include "interfaces/base/model_primitive.hpp"

#include "interfaces/common_objects/asset.hpp"
#include "qry_responses.pb.h"

#include "interfaces/common_objects/asset.hpp"

namespace shared_model {
  /**
   * Provide response with asset
   */
  class AssetResponse : public ModelPrimitive<AssetResponse> {
   public:
    explicit AssetResponse(iroha::protocol::QueryResponse &query_response);

    /**
     * @return Attached asset
     */
    const Asset &asset() const;

    std::string toString() const override;

    bool operator==(const ModelType &rhs) const override;

   private:
    const iroha::protocol::AssetResponse &asset_response_;

    Asset asset_;
  };
}  // namespace shared_model
#endif  // IROHA_SHARED_MODEL_ASSET_RESPONSE_HPP
