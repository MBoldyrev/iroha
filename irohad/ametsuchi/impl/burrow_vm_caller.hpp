/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_AMETSUCHI_BURROW_VM_CALLER_HPP
#define IROHA_AMETSUCHI_BURROW_VM_CALLER_HPP

#include "ametsuchi/vm_caller.hpp"

#include <memory>

namespace iroha::ametsuchi {
  class BurrowVmCaller : public VmCaller {
   public:
    iroha::expected::Result<std::string, std::string> call(
        soci::session &sql,
        std::string const &tx_hash,
        shared_model::interface::types::CommandIndexType cmd_index,
        shared_model::interface::types::EvmCodeHexString const &input,
        shared_model::interface::types::AccountIdType const &caller,
        std::optional<std::reference_wrapper<const std::string>> callee,
        CommandExecutor &command_executor,
        SpecificQueryExecutor &query_executor) const override;
  };
}  // namespace iroha::ametsuchi

#endif