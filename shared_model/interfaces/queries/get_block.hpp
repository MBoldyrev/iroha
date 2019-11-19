/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_GET_BLOCK_HPP
#define IROHA_SHARED_MODEL_GET_BLOCK_HPP

#include "interfaces/base/model_primitive.hpp"

#include "interfaces/common_objects/types.hpp"
#include "queries.pb.h"

namespace shared_model {
  class GetBlock : public ModelPrimitive<GetBlock> {
   public:
    explicit GetBlock(iroha::protocol::Query &query);

    /**
     * Get height of the block to be returned
     * @return block's height
     */
    types::HeightType height() const;

    std::string toString() const override;

    bool operator==(const ModelType &rhs) const override;

   private:
    // ------------------------------| fields |-------------------------------
    const iroha::protocol::GetBlock &get_block_;
  };
}  // namespace shared_model
#endif  // IROHA_SHARED_MODEL_GET_BLOCK_HPP
