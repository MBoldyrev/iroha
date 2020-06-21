/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_MULTIHASH_HASH_TYPE_HPP
#define IROHA_MULTIHASH_HASH_TYPE_HPP

#include <cstdint>

namespace iroha {
  namespace multihash {
    /// https://github.com/multiformats/js-multihash/blob/master/src/constants.js
    enum Type : uint64_t {
      //
      // --- Hash types ---
      //
      sha1 = 0x11,
      sha256 = 0x12,
      sha512 = 0x13,
      blake2s128 = 0xb250,
      blake2s256 = 0xb260,

      //
      // --- public key and signature types ---
      //
      kEd25519Sha2_256 = 0xed,
      kEd25519Sha3_256 = 0xed3256,

      kEcdsaSha2_224 = 0xecd21e,
      kEcdsaSha2_256 = 0xecd220,
      kEcdsaSha2_384 = 0xecd228,
      kEcdsaSha2_512 = 0xecd240,
      kEcdsaSha3_224 = 0xecd31e,
      kEcdsaSha3_256 = 0xecd320,
      kEcdsaSha3_384 = 0xecd328,
      kEcdsaSha3_512 = 0xecd340,

    };
  }  // namespace multihash
}  // namespace iroha

#endif
