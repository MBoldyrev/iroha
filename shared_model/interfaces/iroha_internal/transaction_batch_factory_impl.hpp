/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_TRANSACTION_BATCH_FACTORY_IMPL_HPP
#define IROHA_TRANSACTION_BATCH_FACTORY_IMPL_HPP

#include "interfaces/iroha_internal/transaction_batch_factory.hpp"
#include "validators/transaction_batch_validator.hpp"

#include "obj_counter.hpp"

namespace shared_model {
  namespace interface {

    class TransactionBatchFactoryImpl : public TransactionBatchFactory {
     public:
      using FactoryImplResult =
          FactoryResult<UniquePtrCounter<TransactionBatch>>;

      TransactionBatchFactoryImpl(
          std::shared_ptr<validation::AbstractValidator<TransactionBatch>>
              batch_validator);

      FactoryImplResult createTransactionBatch(
          const types::SharedTxsCollectionType &transactions) const override;

      FactoryImplResult createTransactionBatch(
          SharedPtrCounter<Transaction> transaction) const override;

     private:
      std::shared_ptr<validation::AbstractValidator<TransactionBatch>>
          batch_validator_;
    };

  }  // namespace interface
}  // namespace shared_model

#endif  // IROHA_TRANSACTION_BATCH_FACTORY_IMPL_HPP
