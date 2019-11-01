/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "interfaces/query_responses/transactions_page_response.hpp"
#include "interfaces/transaction.hpp"

using namespace shared_model;

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

std::string TransactionsPageResponse::toString() const {
  auto builder = detail::PrettyStringBuilder()
                     .init("TransactionsPageResponse")
                     .appendAll("transactions",
                                transactions(),
                                [](auto &tx) { return tx.toString(); })
                     .append("all transactions size",
                             std::to_string(allTransactionsSize()));
  if (nextTxHash()) {
    return builder.append("next tx hash", nextTxHash()->hex()).finalize();
  }
  return builder.finalize();
}

bool TransactionsPageResponse::operator==(const ModelType &rhs) const {
  return transactions() == rhs.transactions()
      and nextTxHash() == rhs.nextTxHash()
      and allTransactionsSize() == rhs.allTransactionsSize();
}
