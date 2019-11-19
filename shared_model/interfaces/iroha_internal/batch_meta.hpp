/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_BATCH_META_HPP
#define IROHA_SHARED_MODEL_BATCH_META_HPP

#include "interfaces/base/model_primitive.hpp"
#include "interfaces/common_objects/types.hpp"

namespace shared_model {

  /**
   * Representation of fixed point number
   */
  class BatchMeta : public ModelPrimitive<BatchMeta> {
   public:
    types::BatchType type() const;

    std::string toString() const override {
      return detail::PrettyStringBuilder()
          .init("BatchMeta")
          .append("Type",
                  type() == types::BatchType::ATOMIC ? "ATOMIC" : "ORDERED")
          .appendAll(reducedHashes(),
                     [](auto &hash) { return hash.toString(); })
          .finalize();
    }
    /// type of hashes collection
    using ReducedHashesType = std::vector<types::HashType>;

    /**
     * @return Hashes of transactions to fetch
     */
    const ReducedHashesType &reducedHashes() const;
    /**
     * Checks equality of objects inside
     * @param rhs - other wrapped value
     * @return true, if wrapped objects are same
     */
    bool operator==(const ModelType &rhs) const override {
      return boost::equal(reducedHashes(), rhs.reducedHashes())
          and type() == rhs.type();
    }
  };
}  // namespace shared_model
#endif  // IROHA_SHARED_MODEL_BATCH_META_HPP
