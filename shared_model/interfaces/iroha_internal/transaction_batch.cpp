/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "interfaces/iroha_internal/transaction_batch.hpp"

#include "interfaces/transaction.hpp"
#include "utils/string_builder.hpp"

std::string TransactionBatch::toString() const {
  return detail::PrettyStringBuilder()
      .init("TransactionBatch")
      .appendAll("Transactions",
                 transactions(),
                 [](auto &tx) { return tx->toString(); })
      .finalize();
}
