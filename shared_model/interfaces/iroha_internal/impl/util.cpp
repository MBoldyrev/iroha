/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "interfaces/iroha_internal/util.hpp"

#include <google/protobuf/message.h>
#include <vector>
#include "cryptography/blob.hpp"

using namespace shared_model;

crypto::Blob util::makeBlob(const Message &message) {
  crypto::Blob::Bytes data;
  data.resize(message.ByteSizeLong());
  message.SerializeToArray(data.data(), data.size());
  return crypto::Blob(std::move(data));
}

iroha::expected::Result<types::SharedTxsCollectionType,
                        TransactionFactoryType::Error>
util::deserializeTransactions(
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
