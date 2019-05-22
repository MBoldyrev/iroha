/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_MUTABLE_FACTORY_HPP
#define IROHA_MUTABLE_FACTORY_HPP

#include <memory>

#include <boost/optional.hpp>
#include "ametsuchi/commit_result.hpp"
#include "common/result.hpp"

namespace shared_model {
  namespace interface {
    class Block;
  }
}  // namespace shared_model

namespace iroha {
  namespace ametsuchi {

    class MutableStorage;

    class MutableFactory {
     public:
      /**
       * Creates a mutable storage from the current state.
       * Mutable storage is the only way to commit the block to the ledger.
       * @return Created Result with mutable storage or error string
       */
      virtual expected::Result<std::unique_ptr<MutableStorage>, std::string>
      createMutableStorage() = 0;

      /**
       * Commit mutable storage to Ametsuchi.
       * This transforms Ametsuchi to the new state consistent with
       * MutableStorage.
       * @param mutableStorage
       * @return the status of commit
       */
      virtual CommitResult commit(
          std::unique_ptr<MutableStorage> mutableStorage) = 0;

      /**
       * Try to apply prepared block to Ametsuchi.
       * @return commit result if prepared blocks are enabled, boost::none
       * otherwise. WSV is not changed in this case.
       */
      virtual boost::optional<CommitResult> commitPrepared(
          std::shared_ptr<const shared_model::interface::Block> block) = 0;

      virtual ~MutableFactory() = default;
    };

  }  // namespace ametsuchi
}  // namespace iroha
#endif  // IROHA_MUTABLE_FACTORY_HPP
