/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TEST_INTEGRATION_EXECUTOR_TEST_BLOCK_STORAGE_HPP
#define TEST_INTEGRATION_EXECUTOR_TEST_BLOCK_STORAGE_HPP

#include "ametsuchi/read_only_block_storage.hpp"

struct MockBlock;

namespace iroha {
  namespace integration_framework {

    class TestBlockStorage : public iroha::ametsuchi::ReadOnlyBlockStorage {
     public:
      /// -------------- / ReadOnlyBlockStorage interface \ -------------------

      boost::optional<std::shared_ptr<const shared_model::interface::Block>>
      fetch(shared_model::interface::types::HeightType height) const override;

      size_t size() const override;

      void forEach(FunctionType function) const override;

      /// --------------- \ ReadOnlyBlockStorage interface / -------------------

      /// Add a transaction to mock corresponding mock block.
      void addTransaction(
          shared_model::interface::types::HeightType height,
          size_t index,
          std::shared_ptr<shared_model::interface::Transaction> tx);

     private:
      MockBlock *createMockBlock(
          shared_model::interface::types::HeightType height);

      std::vector<std::shared_ptr<const shared_model::interface::Block>>
          blocks_;
      std::vector<
          std::vector<std::shared_ptr<shared_model::interface::Transaction>>>
          block_storage_transactions_;
    };

  }  // namespace integration_framework
}  // namespace iroha

#endif
