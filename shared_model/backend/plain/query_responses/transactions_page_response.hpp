/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_PLAIN_MODEL_TRANSACTIONS_PAGE_RESPONSE_HPP
#define IROHA_PLAIN_MODEL_TRANSACTIONS_PAGE_RESPONSE_HPP

#include "interfaces/query_responses/transactions_page_response.hpp"

#include <memory>
#include <vector>

#include <boost/optional/optional.hpp>
#include "cryptography/hash.hpp"

namespace shared_model {
  namespace plain {

    class TransactionsPageResponse
        : public shared_model::interface::TransactionsPageResponse {
     public:
      using TransactionsHolder =
          std::vector<std::unique_ptr<shared_model::interface::Transaction>>;

      TransactionsPageResponse(
          TransactionsHolder transactions_page,
          boost::optional<const crypto::Hash &> next_tx_hash,
          shared_model::interface::types::TransactionsNumberType
              all_transactions_size);

      shared_model::interface::types::TransactionsCollectionType transactions()
          const override;

      boost::optional<shared_model::interface::types::HashType> nextTxHash()
          const override;

      shared_model::interface::types::TransactionsNumberType
      allTransactionsSize() const override;

     private:
      std::vector<std::unique_ptr<shared_model::interface::Transaction>>
          transactions_page_;
      boost::optional<shared_model::crypto::Hash> next_tx_hash_;
      shared_model::interface::types::TransactionsNumberType
          all_transactions_size_;
    };
  }  // namespace plain
}  // namespace shared_model
#endif  // IROHA_PLAIN_MODEL_TRANSACTIONS_PAGE_RESPONSE_HPP
