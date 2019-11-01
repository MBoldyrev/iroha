/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "interfaces/iroha_internal/deserialize_repeated_transactions.hpp"

using namespace shared_model;

iroha::expected::Result<types::SharedTxsCollectionType,
                        TransactionFactoryType::Error>
deserializeTransactions(
    const TransactionFactoryType &transaction_factory,
    const google::protobuf::RepeatedPtrField<iroha::protocol::Transaction>
        &transactions) {
  types::SharedTxsCollectionType tx_collection;
  for (const auto &tx : transactions) {
    auto model_tx = transaction_factory.build(tx);
    if (auto e = iroha::expected::resultToOptionalError(model_tx)) {
      return *e;
    }
    tx_collection.emplace_back(
        *iroha::expected::resultToOptionalValue(std::move(model_tx)));
  }
  return tx_collection;
}