/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_CRYPTO_HSM_UTIMACO_CONNECTION_HPP
#define IROHA_CRYPTO_HSM_UTIMACL_CONNECTION_HPP

#include <memory>
#include <mutex>

#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
#include "CXI/include/cxi.h"
#include "interfaces/common_objects/byte_range.hpp"
#include "multihash/type.hpp"

namespace shared_model {
  namespace crypto {
    namespace hsm_utimaco {

      inline cxi::ByteArray irohaToCxiBuffer(
          shared_model::interface::types::ByteRange range) {
        static_assert(sizeof(*range.data()) == sizeof(char), "type mismatch");
        return cxi::ByteArray{reinterpret_cast<char const *>(range.data()),
                              static_cast<int>(range.length())};
      }

      // clang-format off
#define EL0 (iroha::multihash::Type::kEcdsaSha2_224, CXI_MECH_HASH_ALGO_SHA224)
#define EL1 (iroha::multihash::Type::kEcdsaSha2_256, CXI_MECH_HASH_ALGO_SHA256)
#define EL2 (iroha::multihash::Type::kEcdsaSha2_384, CXI_MECH_HASH_ALGO_SHA384)
#define EL3 (iroha::multihash::Type::kEcdsaSha2_512, CXI_MECH_HASH_ALGO_SHA512)
#define EL4 (iroha::multihash::Type::kEcdsaSha3_224, CXI_MECH_HASH_ALGO_SHA3_224)
#define EL5 (iroha::multihash::Type::kEcdsaSha3_256, CXI_MECH_HASH_ALGO_SHA3_256)
#define EL6 (iroha::multihash::Type::kEcdsaSha3_384, CXI_MECH_HASH_ALGO_SHA3_384)
#define EL7 (iroha::multihash::Type::kEcdsaSha3_512, CXI_MECH_HASH_ALGO_SHA3_512)
      // clang-format on

#define NUM_ELEMS 8

#define SWL(z, i, ...)                   \
  case BOOST_PP_TUPLE_ELEM(2, 0, EL##i): \
    return BOOST_PP_TUPLE_ELEM(2, 1, EL##i);
#define SWR(z, i, ...)                   \
  case BOOST_PP_TUPLE_ELEM(2, 1, EL##i): \
    return BOOST_PP_TUPLE_ELEM(2, 0, EL##i);

#define SW_ALL_LEFT(v) \
  switch (v) { BOOST_PP_REPEAT(NUM_ELEMS, SWL, ) }

#define SW_ALL_RIGHT(v) \
  switch (v) { BOOST_PP_REPEAT(NUM_ELEMS, SWR, ) }

      inline std::optional<iroha::multihash::Type>
      CxiHashAlgoToMultihashEd25519Type(int algo) {
        SW_ALL_RIGHT(algo)
        return std::nullopt;
      }

      inline std::optional<int> multihashEd25519ToCxiHashAlgo(
          iroha::multihash::Type type) {
        SW_ALL_LEFT(type)
        return std::nullopt;
      }

    }  // namespace hsm_utimaco
  }    // namespace crypto
}  // namespace shared_model

#endif
