/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "cryptography/ed25519_sha3_impl/signer.hpp"

#include <ed25519_cpp/ed25519.hpp>
#include <ed25519_cpp/sha256.hpp>
#include "common/hexutils.hpp"
#include "crypto/hash_types.hpp"
#include "ed25519/ed25519/sha256.h"

namespace shared_model {
  namespace crypto {
    // TODO IR-716 24.04.2020 mboldyrev: make Signer::sign return a Result
    std::string Signer::sign(const Blob &blob, const Keypair &keypair) {
      return iroha::hexstringToBytestringResult(keypair.publicKey())
          .match(
              [&](auto &&public_key) {
                using namespace iroha_ed25519::v2;
                Sha256 hash;
                if (not sha256(hash, MessageView{blob.range()})) {
                  return std::string{};
                }
                const auto private_key = keypair.privateKey().range();
                Signature signature;
                iroha_ed25519::v2::sign(signature,
                                        MessageView{hash},
                                        PrivateKeyView{private_key},
                                        PublicKeyView{public_key.value});
                return iroha::bytestringToHexstring(
                    shared_model::interface::types::makeByteRange(signature));
              },
              [](const auto & /* error */) { return std::string{}; });
    }
  }  // namespace crypto
}  // namespace shared_model
