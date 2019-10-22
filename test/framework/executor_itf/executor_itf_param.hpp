/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_TEST_FRAMEWORK_EXECUTOR_ITF_PARAM_HPP
#define IROHA_TEST_FRAMEWORK_EXECUTOR_ITF_PARAM_HPP

#include <memory>

namespace iroha {
  namespace ametsuchi {
    class CommandExecutor;
    class Indexer;
    class SpecificQueryExecutor;
  }  // namespace ametsuchi

  namespace integration_framework {

    /// Holder of backend-dependent tested components.
    struct ExecutorItfTarget final {
      std::shared_ptr<iroha::ametsuchi::CommandExecutor> command_executor;
      std::shared_ptr<iroha::ametsuchi::SpecificQueryExecutor> query_executor;
      std::shared_ptr<iroha::ametsuchi::Indexer> tx_indexer;
    };

  }  // namespace integration_framework
}  // namespace iroha

#endif  // IROHA_TEST_FRAMEWORK_EXECUTOR_ITF_PARAM_HPP
