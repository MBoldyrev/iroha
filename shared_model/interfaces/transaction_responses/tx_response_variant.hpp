/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_TX_RESPONSE_VARIANT_HPP
#define IROHA_SHARED_MODEL_TX_RESPONSE_VARIANT_HPP

#include "interfaces/transaction_responses/tx_response.hpp"

#include <boost/variant.hpp>

namespace boost {
  extern template class variant<
      const shared_model::StatelessFailedTxResponse &,
      const shared_model::StatelessValidTxResponse &,
      const shared_model::StatefulFailedTxResponse &,
      const shared_model::StatefulValidTxResponse &,
      const shared_model::RejectedTxResponse &,
      const shared_model::CommittedTxResponse &,
      const shared_model::MstExpiredResponse &,
      const shared_model::NotReceivedTxResponse &,
      const shared_model::MstPendingResponse &,
      const shared_model::EnoughSignaturesCollectedResponse &>;
}

#endif  // IROHA_SHARED_MODEL_TX_RESPONSE_VARIANT_HPP
