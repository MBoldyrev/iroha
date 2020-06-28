/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "cryptography/ed25519_sha3_impl/crypto_provider.hpp"

#include <ed25519_cpp/ed25519.hpp>
#include <ed25519_cpp/randombytes.hpp>
#include <ed25519_cpp/sha256.hpp>
#include "common/hexutils.hpp"
#include "cryptography/ed25519_sha3_impl/signer.hpp"
#include "cryptography/ed25519_sha3_impl/verifier.hpp"

using namespace shared_model::interface::types;

namespace shared_model {
  namespace crypto {

    std::string CryptoProviderEd25519Sha3::sign(const Blob &blob,
                                                const Keypair &keypair) {
      return Signer::sign(blob, keypair);
    }

    bool CryptoProviderEd25519Sha3::verify(SignatureByteRangeView signature,
                                           const Blob &orig,
                                           PublicKeyByteRangeView public_key) {
      return ed25519_sha3::Verifier::verifyEd25519Sha3(
          signature, orig.range(), public_key);
    }

    Seed CryptoProviderEd25519Sha3::generateSeed() {
      unsigned char seed_buf[kSeedLength];
      iroha_ed25519::v2::randombytes(seed_buf, sizeof(seed_buf));
      return Seed(makeByteRange(seed_buf, sizeof(seed_buf)));
    }

    Seed CryptoProviderEd25519Sha3::generateSeed(
        const std::string &passphrase) {
      iroha_ed25519::v2::Sha256 passphrase_hash;
      if (not iroha_ed25519::v2::sha256(
              passphrase_hash, iroha_ed25519::v2::MessageView{passphrase})) {
        return {};
      }
      return Seed(makeByteRange(passphrase_hash));
    }

    Keypair CryptoProviderEd25519Sha3::generateKeypair() {
      return generateKeypair(generateSeed());
    }

    Keypair CryptoProviderEd25519Sha3::generateKeypair(const Seed &seed) {
      assert(seed.size() == kSeedLength);
      iroha_ed25519::v2::PrivateKeyView private_key{seed.range()};
      iroha_ed25519::v2::PublicKey public_key;
      iroha_ed25519::v2::derive_public_key(private_key, public_key);
      return Keypair(PublicKeyHexStringView{iroha::bytestringToHexstring(
                         makeByteRange(public_key))},
                     PrivateKey{makeByteRange(private_key)});
    }

    constexpr size_t CryptoProviderEd25519Sha3::kHashLength;
    constexpr size_t CryptoProviderEd25519Sha3::kPublicKeyLength;
    constexpr size_t CryptoProviderEd25519Sha3::kPrivateKeyLength;
    constexpr size_t CryptoProviderEd25519Sha3::kSignatureLength;
    constexpr size_t CryptoProviderEd25519Sha3::kSeedLength;

    const char *CryptoProviderEd25519Sha3::kName = "Internal Ed25519 with SHA3";
  }  // namespace crypto
}  // namespace shared_model
