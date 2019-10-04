/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_QUERY_EXECUTOR_IMPL_HPP
#define IROHA_QUERY_EXECUTOR_IMPL_HPP

#include "ametsuchi/query_executor.hpp"
#include "logger/logger_fwd.hpp"

namespace shared_model {
  namespace interface {
    class QueryResponseFactory;
  }  // namespace interface
}  // namespace shared_model

namespace iroha {
  namespace ametsuchi {class SpecificQueryExecutor;}

  namespace newstorage {
    class ImmutableWsv;

   class QueryExecutorImpl : public ametsuchi::QueryExecutor {
     public:
        QueryExecutorImpl(
            ImmutableWsv &db,
          std::shared_ptr<shared_model::interface::QueryResponseFactory>
              response_factory,
          std::shared_ptr<ametsuchi::SpecificQueryExecutor> specific_query_executor,
          logger::LoggerPtr log);

      ametsuchi::QueryExecutorResult validateAndExecute(
          const shared_model::interface::Query &query,
          const bool validate_signatories) override;

      bool validate(const shared_model::interface::BlocksQuery &query,
                    const bool validate_signatories) override;

     private:
      template <class Q>
      bool validateSignatures(const Q &query);

      ImmutableWsv &db_;
      std::shared_ptr<ametsuchi::SpecificQueryExecutor> specific_query_executor_;
      std::shared_ptr<shared_model::interface::QueryResponseFactory>
          query_response_factory_;
      logger::LoggerPtr log_;
    };

  }  // namespace newstorage
}  // namespace iroha

#endif  // IROHA_QUERY_EXECUTOR_IMPL_HPP
