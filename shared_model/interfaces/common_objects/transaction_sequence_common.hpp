/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_TRANSACTION_SEQUENCE_COMMON_HPP
#define IROHA_TRANSACTION_SEQUENCE_COMMON_HPP

#include <memory>
#include <vector>

#include <boost/range/any_range.hpp>

#include "obj_counter.hpp"

namespace shared_model {
  namespace interface {

    class Transaction;
    class TransactionBatch;

    namespace types {

      using TransactionsForwardCollectionType =
          boost::any_range<Transaction,
                           boost::forward_traversal_tag,
                           const Transaction &>;

      using SharedTxsCollectionType =
          std::vector<SharedPtrCounter<Transaction>>;

      using BatchesCollectionType =
          std::vector<SharedPtrCounter<TransactionBatch>>;
    }  // namespace types
  }    // namespace interface
}  // namespace shared_model

#endif  // IROHA_TRANSACTION_SEQUENCE_COMMON_HPP
