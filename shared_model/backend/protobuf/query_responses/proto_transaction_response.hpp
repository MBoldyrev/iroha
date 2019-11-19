/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_PROTO_TRANSACTION_RESPONSE_HPP
#define IROHA_SHARED_MODEL_PROTO_TRANSACTION_RESPONSE_HPP

#include "interfaces/query_responses/transactions_response.hpp"

#include "backend/protobuf/transaction.hpp"
#include "interfaces/common_objects/types.hpp"
#include "qry_responses.pb.h"

namespace shared_model {
  class TransactionsResponse final : public TransactionsResponse {
   public:
    explicit TransactionsResponse(
        iroha::protocol::QueryResponse &query_response);

    types::TransactionsCollectionType transactions() const override;

   private:
    const iroha::protocol::TransactionsResponse &transaction_response_;

    const std::vector<Transaction> transactions_;
  };
}  // namespace shared_model

#endif  // IROHA_SHARED_MODEL_PROTO_TRANSACTION_RESPONSE_HPP
