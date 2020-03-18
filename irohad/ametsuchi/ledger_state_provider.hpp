/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_LEDGER_STATE_PROVIDER_HPP
#define IROHA_LEDGER_STATE_PROVIDER_HPP

#include <memory>

#include "ametsuchi/ledger_state.hpp"

namespace iroha {
  class LedgerStateProvider {
   public:
    virtual std::shared_ptr<const LedgerState> get() const;

    virtual void set(std::shared_ptr<const LedgerState> state);
  };
}  // namespace iroha

#endif
