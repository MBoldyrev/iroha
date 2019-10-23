/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_READ_ONLY_BLOCK_STORAGE_HPP
#define IROHA_READ_ONLY_BLOCK_STORAGE_HPP

#include <cstdint>
#include <functional>
#include <memory>

#include <boost/optional.hpp>
#include "interfaces/iroha_internal/block.hpp"

namespace iroha {
  namespace ametsuchi {

    /**
     * Read-only block storage interface
     */
    class ReadOnlyBlockStorage {
     public:
      /**
       * Get block with given height
       * @return block if exists, boost::none otherwise
       */
      virtual boost::optional<
          std::shared_ptr<const shared_model::interface::Block>>
      fetch(shared_model::interface::types::HeightType height) const = 0;

      /**
       * Returns the size of the storage
       */
      virtual size_t size() const = 0;

      /// type of function which can be applied to the elements of the storage
      using FunctionType = std::function<void(
          std::shared_ptr<const shared_model::interface::Block>)>;

      /**
       * Iterates through all the stored blocks
       */
      virtual void forEach(FunctionType function) const = 0;

      virtual ~ReadOnlyBlockStorage() = default;
    };

  }  // namespace ametsuchi
}  // namespace iroha

#endif  // IROHA_READ_ONLY_BLOCK_STORAGE_HPP
