/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_PENDING_TRANSACTION_STORAGE_INIT_HPP
#define IROHA_PENDING_TRANSACTION_STORAGE_INIT_HPP

#include <memory>

#include <rxcpp/rx-lite.hpp>
#include "interfaces/common_objects/types.hpp"

namespace shared_model {
  class TransactionBatch;
}  // namespace shared_model

namespace iroha {

  class MstProcessor;
  class MstState;
  class PendingTransactionStorage;

  namespace network {
    class PeerCommunicationService;
  }

  class PendingTransactionStorageInit {
   public:
    PendingTransactionStorageInit();

    std::shared_ptr<PendingTransactionStorage>
    createPendingTransactionsStorage();

    void setSubscriptions(const MstProcessor &mst_processor);

    void setSubscriptions(
        const network::PeerCommunicationService &peer_communication_service);

    ~PendingTransactionStorageInit();

   protected:
    rxcpp::composite_subscription pending_storage_lifetime;
    rxcpp::subjects::subject<std::shared_ptr<iroha::MstState>> updated_batches;
    rxcpp::subjects::subject<std::shared_ptr<shared_model::TransactionBatch>>
        prepared_batch;
    rxcpp::subjects::subject<std::shared_ptr<shared_model::TransactionBatch>>
        expired_batch;
    rxcpp::subjects::subject<std::pair<shared_model::types::AccountIdType,
                                       shared_model::types::HashType>>
        prepared_txs;
  };
}  // namespace iroha

#endif  // IROHA_PENDING_TRANSACTION_STORAGE_INIT_HPP
