/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SYNCHRONIZER_COMMON_HPP
#define IROHA_SYNCHRONIZER_COMMON_HPP

#include <utility>

#include <rxcpp/rx.hpp>

#include "ametsuchi/ledger_state.hpp"
#include "consensus/types.hpp"
#include "interfaces/common_objects/types.hpp"
#include "interfaces/iroha_internal/block.hpp"

namespace iroha {
  namespace synchronizer {

    /**
     * Chain of block(s), which was either committed directly by this peer or
     * downloaded from another; contains zero or more blocks depending on
     * synchronization outcome
     */
    using Chain =
        rxcpp::observable<std::shared_ptr<shared_model::interface::Block>>;

    /**
     * Event, which is emitted by synchronizer, when it receives and processes
     * commit
     */
    struct SynchronizationEvent {
      // TODO andrei 08.11.2018 IR-1852 Rework blocks collection from
      // synchronizer with iterable
      Chain synced_blocks;
      iroha::consensus::OutcomeType sync_outcome;
      shared_model::interface::types::HeightType height;
      consensus::Round round;
      std::shared_ptr<iroha::LedgerState> ledger_state;
    };

  }  // namespace synchronizer
}  // namespace iroha

#endif  // IROHA_SYNCHRONIZER_COMMON_HPP
