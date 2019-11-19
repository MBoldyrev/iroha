/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_STATEFUL_VALID_TX_RESPONSE_HPP
#define IROHA_STATEFUL_VALID_TX_RESPONSE_HPP

#include "interfaces/transaction_responses/abstract_tx_response.hpp"

namespace shared_model {
  /**
   * Tx response of passed stateful validation
   */
  class StatefulValidTxResponse
      : public AbstractTxResponse<StatefulValidTxResponse> {
   private:
    std::string className() const override {
      return "StatefulValidTxResponse";
    }
  };

}  // namespace shared_model
#endif  // IROHA_STATEFUL_VALID_TX_RESPONSE_HPP
