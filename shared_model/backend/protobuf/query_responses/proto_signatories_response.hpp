/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_PROTO_SIGNATORIES_RESPONSE_HPP
#define IROHA_SHARED_MODEL_PROTO_SIGNATORIES_RESPONSE_HPP

#include "interfaces/query_responses/signatories_response.hpp"

#include "cryptography/public_key.hpp"
#include "qry_responses.pb.h"

namespace shared_model {
  class SignatoriesResponse final : public SignatoriesResponse {
   public:
    explicit SignatoriesResponse(
        iroha::protocol::QueryResponse &query_response);

    const types::PublicKeyCollectionType &keys() const override;

   private:
    const iroha::protocol::SignatoriesResponse &signatories_response_;

    const types::PublicKeyCollectionType keys_;
  };
}  // namespace shared_model

#endif  // IROHA_SHARED_MODEL_PROTO_SIGNATORIES_RESPONSE_HPP
