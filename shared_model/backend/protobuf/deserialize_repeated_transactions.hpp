/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_PROTO_DESERIALIZE_REPEATED_TRANSACTIONS_HPP
#define IROHA_SHARED_MODEL_PROTO_DESERIALIZE_REPEATED_TRANSACTIONS_HPP

#include "common/result.hpp"
#include "interfaces/common_objects/transaction_sequence_common.hpp"
#include "interfaces/iroha_internal/abstract_transport_factory.hpp"
#include "interfaces/transaction.hpp"
#include "transaction.pb.h"

namespace shared_model {

  using TransactionFactoryType =
      AbstractTransportFactory<shared_model::Transaction,
                               iroha::protocol::Transaction>;

  iroha::expected::Result<types::SharedTxsCollectionType,
                          TransactionFactoryType::Error>
  deserializeTransactions(
      const TransactionFactoryType &transaction_factory,
      const google::protobuf::RepeatedPtrField<iroha::protocol::Transaction>
          &transactions);
}  // namespace shared_model

#endif  // IROHA_SHARED_MODEL_PROTO_DESERIALIZE_REPEATED_TRANSACTIONS_HPP
