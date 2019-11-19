/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_PROTO_TRANSACTION_PAGE_RESPONSE_HPP
#define IROHA_SHARED_MODEL_PROTO_TRANSACTION_PAGE_RESPONSE_HPP

#include "interfaces/query_responses/transactions_page_response.hpp"

#include "backend/protobuf/transaction.hpp"
#include "interfaces/common_objects/types.hpp"
#include "qry_responses.pb.h"

namespace shared_model {
  class TransactionsPageResponse final : public TransactionsPageResponse {
   public:
    explicit TransactionsPageResponse(
        iroha::protocol::QueryResponse &query_response);

    types::TransactionsCollectionType transactions() const override;

    boost::optional<types::HashType> nextTxHash() const override;

    types::TransactionsNumberType allTransactionsSize() const override;

   private:
    const iroha::protocol::TransactionsPageResponse &transactionPageResponse_;
    std::vector<Transaction> transactions_;
    boost::optional<types::HashType> next_hash_;
  };
}  // namespace shared_model

#endif  // IROHA_SHARED_MODEL_PROTO_TRANSACTION_PAGE_RESPONSE_HPP
