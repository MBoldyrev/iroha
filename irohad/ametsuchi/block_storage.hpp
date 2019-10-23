/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_BLOCK_STORAGE_HPP
#define IROHA_BLOCK_STORAGE_HPP

#include "ametsuchi/read_only_block_storage.hpp"

#include <cstdint>
#include <memory>

#include "interfaces/iroha_internal/block.hpp"

namespace iroha {
  namespace ametsuchi {

    /**
     * Append-only block storage interface
     */
    class BlockStorage : public ReadOnlyBlockStorage {
     public:
      /**
       * Append block, if the storage doesn't already contain the same block
       * @return true if inserted successfully, false otherwise
       */
      virtual bool insert(
          std::shared_ptr<const shared_model::interface::Block> block) = 0;

      /**
       * Clears the contents of storage
       */
      virtual void clear() = 0;
    };

  }  // namespace ametsuchi
}  // namespace iroha

#endif  // IROHA_BLOCK_STORAGE_HPP
