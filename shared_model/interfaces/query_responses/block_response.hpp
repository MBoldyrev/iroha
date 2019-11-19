/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_BLOCK_RESPONSE_HPP
#define IROHA_SHARED_MODEL_BLOCK_RESPONSE_HPP

#include "interfaces/base/model_primitive.hpp"

#include "interfaces/common_objects/types.hpp"

namespace shared_model {

  class Block;

  /**
   * Provide response with block
   */
  class BlockResponse : public ModelPrimitive<BlockResponse> {
   public:
    /**
     * @return Attached block
     */
    const Block &block() const;

    /**
     * Stringify the data.
     * @return string representation of data.
     */
    std::string toString() const override;

    /**
     * @return true if the data are same.
     */
    bool operator==(const ModelType &rhs) const override;
  };
}  // namespace shared_model
#endif  // IROHA_SHARED_MODEL_BLOCK_RESPONSE_HPP
