/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_QUERY_PROCESSOR_IMPL_HPP
#define IROHA_QUERY_PROCESSOR_IMPL_HPP

#include "torii/processor/query_processor.hpp"

#include <rxcpp/rx-lite.hpp>
#include "ametsuchi/storage.hpp"
#include "interfaces/iroha_internal/query_response_factory.hpp"
#include "logger/logger_fwd.hpp"

namespace iroha {
  namespace torii {

    /**
     * QueryProcessorImpl provides implementation of QueryProcessor
     */
    class QueryProcessorImpl : public QueryProcessor {
     public:
      QueryProcessorImpl(
          std::shared_ptr<ametsuchi::Storage> storage,
          std::shared_ptr<ametsuchi::QueryExecutorFactory> qry_exec,
          std::shared_ptr<iroha::PendingTransactionStorage>
              pending_transactions,
          std::shared_ptr<shared_model::QueryResponseFactory> response_factory,
          logger::LoggerPtr log);

      std::unique_ptr<shared_model::QueryResponse> queryHandle(
          const shared_model::Query &qry) override;

      rxcpp::observable<std::shared_ptr<shared_model::BlockQueryResponse>>
      blocksQueryHandle(const shared_model::BlocksQuery &qry) override;

     private:
      rxcpp::subjects::subject<
          std::shared_ptr<shared_model::BlockQueryResponse>>
          blocks_query_subject_;
      std::shared_ptr<ametsuchi::Storage> storage_;
      std::shared_ptr<ametsuchi::QueryExecutorFactory> qry_exec_;
      std::shared_ptr<iroha::PendingTransactionStorage> pending_transactions_;
      std::shared_ptr<shared_model::QueryResponseFactory> response_factory_;

      logger::LoggerPtr log_;
    };

  }  // namespace torii
}  // namespace iroha

#endif  // IROHA_QUERY_PROCESSOR_IMPL_HPP
