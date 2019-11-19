/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_BLOCK_INDEX_HPP
#define IROHA_BLOCK_INDEX_HPP

#include <memory>

namespace shared_model {
  class Block;
}  // namespace shared_model

namespace iroha {
  namespace ametsuchi {
    /**
     * Internal interface for modifying index on blocks and transactions
     */
    class BlockIndex {
     public:
      virtual ~BlockIndex() = default;

      /**
       * Create necessary indexes for block
       * @param block to be indexed
       */
      virtual void index(const shared_model::Block &) = 0;
    };
  }  // namespace ametsuchi
}  // namespace iroha

#endif  // IROHA_BLOCK_INDEX_HPP
