/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_PROTO_BATCH_META_HPP
#define IROHA_PROTO_BATCH_META_HPP

#include "interfaces/iroha_internal/batch_meta.hpp"

#include <boost/range/adaptor/transformed.hpp>
#include "cryptography/hash.hpp"
#include "interfaces/common_objects/types.hpp"
#include "transaction.pb.h"

namespace shared_model {
  class BatchMeta final : public BatchMeta {
   public:
    explicit BatchMeta(
        iroha::protocol::Transaction::Payload::BatchMeta &batch_meta)
        : batch_meta_{batch_meta},
          type_{[this] {
            unsigned which = batch_meta_.GetDescriptor()
                                 ->FindFieldByName("type")
                                 ->enum_type()
                                 ->FindValueByNumber(batch_meta_.type())
                                 ->index();
            return static_cast<types::BatchType>(which);
          }()},
          reduced_hashes_{boost::copy_range<ReducedHashesType>(
              batch_meta.reduced_hashes()
              | boost::adaptors::transformed([](const auto &hash) {
                  return crypto::Hash::fromHexString(hash);
                }))} {}

    types::BatchType type() const override {
      return type_;
    }

    const ReducedHashesType &reducedHashes() const override {
      return reduced_hashes_;
    }

   private:
    const iroha::protocol::Transaction::Payload::BatchMeta &batch_meta_;

    types::BatchType type_;

    const ReducedHashesType reduced_hashes_;
  };
}  // namespace shared_model
#endif  // IROHA_PROTO_BATCH_META_HPP
