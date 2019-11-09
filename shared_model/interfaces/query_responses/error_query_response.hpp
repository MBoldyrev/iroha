/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_QUERY_ERROR_RESPONSE_HPP
#define IROHA_SHARED_MODEL_QUERY_ERROR_RESPONSE_HPP

#include "interfaces/base/model_primitive.hpp"
#include "interfaces/iroha_internal/error_query_response_reason.hpp"

#include "qry_responses.pb.h"

namespace shared_model {

  /**
   * QueryErrorResponse interface container for all concrete error responses
   * possible achieved in the system.
   */
  class ErrorQueryResponse : public ModelPrimitive<ErrorQueryResponse> {
   public:
    explicit ErrorQueryResponse(iroha::protocol::QueryResponse &query_response);

    /**
     * @return general error reason
     */
    QueryErrorType reason() const;

    /// Message type
    using ErrorMessageType = std::string;

    /**
     * @return error message if present, otherwise - an empty string
     */
    const ErrorMessageType &errorMessage() const;

    /**
     * @return stateful error code of this query response:
     *    0 - error is in query's type, it is not a stateful one
     *    1 - internal error
     *    2 - not enough permissions
     *    3 - invalid signatures
     */
    QueryErrorCodeType errorCode() const;

    // ------------------------| Primitive override |-------------------------

    std::string toString() const override;

    bool operator==(const ModelType &rhs) const override;

   private:
    iroha::protocol::ErrorResponse &error_response_;
    QueryErrorType error_reason_;
    const ErrorMessageType &error_message_;
    QueryErrorCodeType error_code_;
  };
}  // namespace shared_model
#endif  // IROHA_SHARED_MODEL_QUERY_ERROR_RESPONSE_HPP
