/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_AMETSUCHI_H
#define IROHA_AMETSUCHI_H

#include <vector>

#include <rxcpp/rx-observable-fwd.hpp>
#include "ametsuchi/commit_result.hpp"
#include "common/result.hpp"

namespace shared_model {
  namespace interface {
    class Block;
  }
}  // namespace shared_model

namespace iroha {

  namespace ametsuchi {

    class BlockStorageFactory;
    class BlockQuery;
    class PeerQuery;
    class WsvQuery;
    class MutableStorage;
    class TemporaryWsv;
    class QueryExecutor;
    class SettingQuery;
    class Storage;

    class StorageFactory {
     public:
      virtual ~StorageFactory() = default;
      virtual std::shared_ptr<Storage> create() = 0;
    };

    /**
     * Storage interface, which allows queries on current committed state, and
     * creation of state which can be mutated with blocks and transactions
     */
    class Storage {
     public:
      virtual std::shared_ptr<WsvQuery> getWsvQuery() const = 0;

      virtual std::shared_ptr<BlockQuery> getBlockQuery() const = 0;

      virtual std::shared_ptr<PeerQuery> getPeerQuery() const = 0;

      virtual boost::optional<std::unique_ptr<SettingQuery>>
      createSettingQuery() const = 0;

      /**
       * Creates a temporary world state view from the current state.
       * Temporary state will be not committed and will be erased on destructor
       * call.
       * Temporary state might be used for transaction validation.
       * @return Created temporary wsv.
       */
      virtual std::unique_ptr<TemporaryWsv> createTemporaryWsv() = 0;

      /**
       * Prepare state which was accumulated in temporary WSV.
       * After preparation, this state is not visible until commited.
       *
       * @param wsv - state which will be prepared.
       */
      virtual void prepareBlock(std::unique_ptr<TemporaryWsv> wsv) = 0;

      virtual boost::optional<std::shared_ptr<PeerQuery>> createPeerQuery()
          const = 0;

      // reuse!
      //virtual void releaseTemporaryWsv(std::unique_ptr<TemporaryWsv> wsv) = 0;

      /**
       * Creates a mutable storage from the current state
       * @return Created mutable storage.
       */
      virtual std::unique_ptr<MutableStorage> createMutableStorage(
          BlockStorageFactory &storage_factory) = 0;

      virtual std::unique_ptr<MutableStorage> createMutableStorage() = 0;

      // reuse!
      virtual void releaseMutableStorage(std::unique_ptr<MutableStorage> st) = 0;

      /**
       * Commit mutable storage to Ametsuchi.
       * This transforms Ametsuchi to the new state consistent with
       * MutableStorage.
       * @param mutableStorage
       * @return the status of commit
       */
      virtual CommitResult commit(
          std::unique_ptr<MutableStorage> mutableStorage) = 0;

      /// Check if prepared commits are enabled.
      virtual bool preparedCommitEnabled() const = 0;

      /**
       * Try to apply prepared block to Ametsuchi.
       * @param block The previously prepared block that will be committed now.
       * @return Result of committing the prepared block.
       */
      virtual CommitResult commitPrepared(
          std::shared_ptr<const shared_model::interface::Block> block) = 0;

      virtual std::shared_ptr<QueryExecutor> createQueryExecutor() const = 0;

      /**
       * method called when block is written to the storage
       * @return observable with the Block committed
       */
      virtual rxcpp::observable<
          std::shared_ptr<const shared_model::interface::Block>>
      on_commit() = 0;

      virtual ~Storage() = default;
    };

  }  // namespace ametsuchi

}  // namespace iroha

#endif  // IROHA_AMETSUCHI_H
