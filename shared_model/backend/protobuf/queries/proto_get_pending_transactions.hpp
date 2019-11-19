/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_PROTO_GET_PENDING_TRANSACTIONS_HPP
#define IROHA_PROTO_GET_PENDING_TRANSACTIONS_HPP

#include "interfaces/queries/get_pending_transactions.hpp"

#include <boost/optional.hpp>
#include "backend/protobuf/queries/proto_tx_pagination_meta.hpp"
#include "queries.pb.h"

namespace shared_model {
  class GetPendingTransactions final : public GetPendingTransactions {
   public:
    explicit GetPendingTransactions(iroha::protocol::Query &query);

    boost::optional<const TxPaginationMeta &> paginationMeta() const override;

   private:
    const iroha::protocol::GetPendingTransactions &pending_transactions_;
    boost::optional<const TxPaginationMeta> pagination_meta_;
  };
}  // namespace shared_model

#endif  // IROHA_PROTO_GET_PENDING_TRANSACTIONS_HPP
