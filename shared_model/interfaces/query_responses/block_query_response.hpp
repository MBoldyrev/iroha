/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_BLOCK_QUERY_RESPONSE_HPP
#define IROHA_SHARED_MODEL_BLOCK_QUERY_RESPONSE_HPP

#include "interfaces/base/model_primitive.hpp"

#include "qry_responses.pb.h"

#include <boost/variant.hpp>

namespace shared_model {

  class BlockResponse;
  class BlockErrorResponse;

  /**
   * Class BlockQueryResponse(qr) provides container with concrete block query
   * responses available in the system. General note: this class is container
   * for QRs but not a base class.
   */
  class BlockQueryResponse : public ModelPrimitive<BlockQueryResponse> {
   private:
    /// const reference shortcut type
    template <typename... Value>
    using ConstRefVariant = boost::variant<const Value &...>;

   public:
    using TransportType = iroha::protocol::BlockQueryResponse;

    explicit BlockQueryResponse(TransportType &&block_query_response);

    /// Type of container with all concrete query response
    using QueryResponseVariantType =
        ConstRefVariant<BlockResponse, BlockErrorResponse>;

    /// Type of all available query responses
    using QueryResponseListType = QueryResponseVariantType::types;

    /**
     * @return reference to const variant with concrete qr
     */
    const QueryResponseVariantType &get() const;

    // ------------------------| Primitive override |-------------------------

    std::string toString() const override;

    bool operator==(const ModelType &rhs) const override;

   private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
  };
}  // namespace shared_model
#endif  // IROHA_SHARED_MODEL_BLOCK_QUERY_RESPONSE_HPP
