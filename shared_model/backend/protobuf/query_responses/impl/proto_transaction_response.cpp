/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "backend/protobuf/query_responses/proto_transaction_response.hpp"

TransactionsResponse::TransactionsResponse(
    iroha::protocol::QueryResponse &query_response)
    : transaction_response_{query_response.transactions_response()},
      transactions_{transaction_response_.transactions().begin(),
                    transaction_response_.transactions().end()} {}

interface::types::TransactionsCollectionType
TransactionsResponse::transactions() const {
  return transactions_;
}
