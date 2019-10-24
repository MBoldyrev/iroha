/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "backend/plain/query_responses/transactions_page_response.hpp"

#include <boost/range/adaptor/indirected.hpp>
#include "interfaces/transaction.hpp"

using namespace shared_model::interface::types;

using shared_model::plain::TransactionsPageResponse;

TransactionsPageResponse::TransactionsPageResponse(
    TransactionsHolder transactions_page,
    boost::optional<const crypto::Hash &> next_tx_hash,
    TransactionsNumberType all_transactions_size)
    : transactions_page_(std::move(transactions_page)),
      next_tx_hash_(std::move(next_tx_hash)),
      all_transactions_size_(all_transactions_size) {}

TransactionsCollectionType TransactionsPageResponse::transactions() const {
  return transactions_page_ | boost::adaptors::indirected;
}

boost::optional<HashType> TransactionsPageResponse::nextTxHash() const {
  return next_tx_hash_;
}

TransactionsNumberType TransactionsPageResponse::allTransactionsSize() const {
  return all_transactions_size_;
}
