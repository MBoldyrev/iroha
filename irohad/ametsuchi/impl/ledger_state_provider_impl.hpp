/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_LEDGER_STATE_PROVIDER_IMPL_HPP
#define IROHA_LEDGER_STATE_PROVIDER_IMPL_HPP

#include "ametsuchi/ledger_state_provider.hpp"

namespace iroha {
  class LedgerStateProviderImpl {
   public:
    std::shared_ptr<LedgerState> get() const override;

    void set(std::shared_ptr<LedgerState> state) override;

   private:
    std::shared_ptr<LedgerState> state_;
  };
}  // namespace iroha

#endif
