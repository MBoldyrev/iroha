/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "interfaces/query_responses/transactions_response.hpp"
#include "interfaces/transaction.hpp"
#include "utils/string_builder.hpp"

std::string TransactionsResponse::toString() const {
  return detail::PrettyStringBuilder()
      .init("TransactionsResponse")
      .appendAll(transactions(), [](auto &tx) { return tx.toString(); })
      .finalize();
}

bool TransactionsResponse::operator==(const ModelType &rhs) const {
  return transactions() == rhs.transactions();
}
