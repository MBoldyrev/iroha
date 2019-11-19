/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "backend/protobuf/query_responses/proto_transactions_page_response.hpp"
#include "common/byteutils.hpp"

TransactionsPageResponse::TransactionsPageResponse(
    iroha::protocol::QueryResponse &query_response)
    : transactionPageResponse_{query_response.transactions_page_response()},
      transactions_{transactionPageResponse_.transactions().begin(),
                    transactionPageResponse_.transactions().end()},
      next_hash_{[this]() -> boost::optional<types::HashType> {
        switch (transactionPageResponse_.next_page_tag_case()) {
          case iroha::protocol::TransactionsPageResponse::kNextTxHash:
            return crypto::Hash::fromHexString(
                transactionPageResponse_.next_tx_hash());
          default:
            return boost::none;
        }
      }()} {}

types::TransactionsCollectionType TransactionsPageResponse::transactions()
    const {
  return transactions_;
}

boost::optional<types::HashType> TransactionsPageResponse::nextTxHash() const {
  return next_hash_;
}

types::TransactionsNumberType TransactionsPageResponse::allTransactionsSize()
    const {
  return transactionPageResponse_.all_transactions_size();
}
