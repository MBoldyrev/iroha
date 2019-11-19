/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_BLOCK_ERROR_RESPONSE_HPP
#define IROHA_SHARED_MODEL_BLOCK_ERROR_RESPONSE_HPP

#include "interfaces/base/model_primitive.hpp"

#include "qry_responses.pb.h"

#include "interfaces/common_objects/types.hpp"

namespace shared_model {
  /**
   * Provide response with error
   */
  class BlockErrorResponse : public ModelPrimitive<BlockErrorResponse> {
   public:
    explicit BlockErrorResponse(
        iroha::protocol::BlockQueryResponse &block_query_response);

    /**
     * @return Attached error message
     */
    const types::DescriptionType &message() const;

    /**
     * Give string description of data.
     * @return string representation of data.
     */
    std::string toString() const override;

    /**
     * @return true if the data are same.
     */
    bool operator==(const ModelType &rhs) const override;

   private:
    const iroha::protocol::BlockErrorResponse &block_error_response;

    const types::DescriptionType message_;
  };
}  // namespace shared_model
#endif  // IROHA_SHARED_MODEL_BLOCK_ERROR_RESPONSE_HPP
