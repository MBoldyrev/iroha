/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_MOCK_QUERY_EXECUTOR_HPP
#define IROHA_MOCK_QUERY_EXECUTOR_HPP

#include "ametsuchi/query_executor.hpp"

#include <gmock/gmock.h>

namespace iroha {
  namespace ametsuchi {

    class MockQueryExecutor : public QueryExecutor {
     public:
      MOCK_METHOD1(validateAndExecute_,
                   shared_model::QueryResponse *(const shared_model::Query &));
      QueryExecutorResult validateAndExecute(
          const shared_model::Query &q,
          bool validate_signatories = true) override {
        return QueryExecutorResult(validateAndExecute_(q));
      }
      MOCK_METHOD2(validate,
                   bool(const shared_model::BlocksQuery &,
                        const bool validate_signatories));
    };

  }  // namespace ametsuchi
}  // namespace iroha

#endif  // IROHA_MOCK_QUERY_EXECUTOR_HPP
