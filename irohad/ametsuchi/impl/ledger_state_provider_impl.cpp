/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ametsuchi/impl/ledger_state_provider_impl.hpp"

#include <atomic>

namespace iroha {
  std::shared_ptr<LedgerState> LedgerStateProviderImpl::get() const {
    return std::atomic_load_explicit(&state_, std::memory_order_acquire);
  }

  void LedgerStateProviderImpl::set(std::shared_ptr<LedgerState> state) {
    return std::atomic_store_explicit(&state_, std::memory_order_release);
  }
}  // namespace iroha

#endif
