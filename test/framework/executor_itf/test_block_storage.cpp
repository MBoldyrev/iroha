/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "framework/executor_itf/test_block_storage.hpp"

#include <algorithm>

#include <boost/range/adaptor/indirected.hpp>
#include "module/shared_model/interface_mocks.hpp"

using namespace iroha::integration_framework;
using namespace shared_model::interface::types;

boost::optional<std::shared_ptr<const shared_model::interface::Block>>
TestBlockStorage::fetch(HeightType height) const {
  if (blocks_.size() < height) {
    return boost::none;
  }
  return blocks_[height];
}

size_t TestBlockStorage::size() const {
  return blocks_.size();
}

void TestBlockStorage::forEach(FunctionType function) const {
  std::for_each(blocks_.begin(), blocks_.end(), function);
}

void TestBlockStorage::addTransaction(
    HeightType height,
    size_t index,
    std::shared_ptr<shared_model::interface::Transaction> tx) {
  auto get_element_ensure_size =
      [](auto &container,
         size_t index) -> std::add_lvalue_reference_t<decltype(container[0])> {
    if (container.size() < index + 1) {
      container.resize(index + 1);
    }
    return container[index];
  };
  auto &block_txs =
      get_element_ensure_size(block_storage_transactions_, height);
  get_element_ensure_size(block_txs, index) = tx;

  while (blocks_.size() <= height) {
    blocks_.emplace_back(createMockBlock(height));
  }
}

MockBlock *TestBlockStorage::createMockBlock(HeightType height) {
  auto mock_block = new MockBlock();
  EXPECT_CALL(*mock_block, clone())
      .WillRepeatedly(::testing::Invoke(
          [this, height] { return this->createMockBlock(height); }));
  EXPECT_CALL(*mock_block, transactions())
      .WillRepeatedly(::testing::Invoke([this, height] {
        return this->block_storage_transactions_[height]
            | boost::adaptors::indirected;
      }));
  return mock_block;
}
