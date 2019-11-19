/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_SIGNATORIES_RESPONSE_HPP
#define IROHA_SHARED_MODEL_SIGNATORIES_RESPONSE_HPP

#include "interfaces/base/model_primitive.hpp"

#include "cryptography/public_key.hpp"
#include "qry_responses.pb.h"

#include "interfaces/common_objects/types.hpp"

namespace shared_model {
  /**
   * Container of asset, for fetching data.
   */
  class SignatoriesResponse : public ModelPrimitive<SignatoriesResponse> {
   public:
    explicit SignatoriesResponse(
        iroha::protocol::QueryResponse &query_response);

    /**
     * @return All public keys attached to account
     */
    const types::PublicKeyCollectionType &keys() const;

    std::string toString() const override;

    bool operator==(const ModelType &rhs) const override;

   private:
    const iroha::protocol::SignatoriesResponse &signatories_response_;

    const types::PublicKeyCollectionType keys_;
  };
}  // namespace shared_model
#endif  // IROHA_SHARED_MODEL_SIGNATORIES_RESPONSE_HPP
