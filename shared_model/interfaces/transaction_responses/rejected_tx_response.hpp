/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_REJECTED_TX_RESPONSE_HPP
#define IROHA_REJECTED_TX_RESPONSE_HPP

#include "interfaces/transaction_responses/abstract_tx_response.hpp"

namespace shared_model {
  /**
   * Status shows that transaction was rejected on consensus
   */
  class RejectedTxResponse : public AbstractTxResponse<RejectedTxResponse> {
   private:
    std::string className() const override {
      return "RejectedTxResponse";
    }
  };

}  // namespace shared_model
#endif  // IROHA_REJECTED_TX_RESPONSE_HPP
