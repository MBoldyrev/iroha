/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "query_executor_impl.hpp"
#include "immutable_wsv.hpp"
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/size.hpp>
#include "ametsuchi/specific_query_executor.hpp"
#include "cryptography/public_key.hpp"
#include "interfaces/iroha_internal/query_response_factory.hpp"
#include "interfaces/queries/blocks_query.hpp"
#include "interfaces/queries/query.hpp"
#include "logger/logger.hpp"

using namespace shared_model::interface::permissions;

namespace iroha {
  namespace newstorage {

    QueryExecutorImpl::QueryExecutorImpl(
        ImmutableWsv &db,
        std::shared_ptr<shared_model::interface::QueryResponseFactory>
            response_factory,
        std::shared_ptr<ametsuchi::SpecificQueryExecutor> specific_query_executor,
        logger::LoggerPtr log)
        : db_(db),
          specific_query_executor_(std::move(specific_query_executor)),
          query_response_factory_{std::move(response_factory)},
          log_(std::move(log)) {}

    template <class Q>
    bool QueryExecutorImpl::validateSignatures(const Q &query) {
      if (boost::size(query.signatures()) != 1) {
        return false;
      }

      uint16_t quorum = 0;
      const auto& signatories = db_.loadAccountSignatoriesAndQuorum(
          query.creatorAccountId(), quorum);

      return !signatories.empty() &&
        signatories.count((*std::begin(query.signatures())).publicKey().hex());
    }

    ametsuchi::QueryExecutorResult QueryExecutorImpl::validateAndExecute(
        const shared_model::interface::Query &query,
        const bool validate_signatories = true) {
      if (validate_signatories and not validateSignatures(query)) {
        // TODO [IR-1816] Akvinikym 03.12.18: replace magic number 3
        // with a named constant
        return query_response_factory_->createErrorQueryResponse(
            shared_model::interface::QueryResponseFactory::ErrorQueryType::
                kStatefulFailed,
            "query signatories did not pass validation",
            3,
            query.hash());
      }
      return specific_query_executor_->execute(query);
    }

    bool QueryExecutorImpl::validate(
        const shared_model::interface::BlocksQuery &query,
        const bool validate_signatories = true) {
      if (validate_signatories and not validateSignatures(query)) {
        log_->error("query signatories did not pass validation");
        return false;
      }
      if (not specific_query_executor_->hasAccountRolePermission(
              Role::kGetBlocks, query.creatorAccountId())) {
        log_->error("query creator does not have enough permissions");
        return false;
      }

      return true;
    }

  }  // namespace newstorage
}  // namespace iroha
