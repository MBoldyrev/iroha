/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "cryptography/hsm_utimaco/crypto_verifier.hpp"

#include <fmt/format.h>
#include "CXI/include/cxi.h"
#include "common/hexutils.hpp"
#include "common/result.hpp"
#include "cryptography/hsm_utimaco/common.hpp"
#include "cryptography/hsm_utimaco/formatters.hpp"

using namespace shared_model::crypto::hsm_utimaco;
using namespace shared_model::interface::types;

namespace {

  iroha::expected::Result<cxi::ByteArray, std::string> makeCxiKeyImportBlob(
      iroha::multihash::Type type, PublicKeyHexStringView public_key) {
    // this is precompiled blob for ed25519 public keys import,
    // other formats need different ones
    char const kEcdsaEd25519ImportBase[] = {
        0x4b, 0x42, 0x00, 0x00, 0x00, 0x59, 0x42, 0x48, 0x00, 0x00, 0x00,
        0x27, 0x50, 0x4c, 0x00, 0x00, 0x00, 0x21, 0x00, 0x03, 0x00, 0x04,
        0x00, 0x00, 0x00, 0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 0x00, 0x00,
        0xff, 0x00, 0x1e, 0x00, 0x0d, 0x65, 0x64, 0x77, 0x61, 0x72, 0x64,
        0x73, 0x32, 0x35, 0x35, 0x31, 0x39, 0x00, 0x4b, 0x43, 0x00, 0x00,
        0x00, 0x26, 0x50, 0x4b, 0x00, 0x00, 0x00, 0x20};
    const size_t kEcdsaEd25519ImportBaseSize =
        std::extent_v<decltype(kEcdsaEd25519ImportBase)>;

    switch (type) {
      case iroha::multihash::Type::kEcdsaSha2_224:
      case iroha::multihash::Type::kEcdsaSha2_256:
      case iroha::multihash::Type::kEcdsaSha2_384:
      case iroha::multihash::Type::kEcdsaSha2_512:
      case iroha::multihash::Type::kEcdsaSha3_224:
      case iroha::multihash::Type::kEcdsaSha3_256:
      case iroha::multihash::Type::kEcdsaSha3_384:
      case iroha::multihash::Type::kEcdsaSha3_512: {
        const auto public_key_binary_size =
            iroha::hexstringToBytestringSize(public_key);
        if (public_key_binary_size != kPublicKeySize) {
          return iroha::expected::makeError(fmt::format(
              "Wrong public key size: {}.", public_key_binary_size));
        }
        const size_t kPublicKeySize = 32;
        return iroha::hexstringToBytestringResult(public_key) |
                   [&](std::string const &public_key)
                   -> iroha::expected::Result<cxi::ByteArray, std::string> {
          cxi::ByteArray import_blob{kEcdsaEd25519ImportBase,
                                     kEcdsaEd25519ImportBaseSize};
          assert(public_key.size() == kPublicKeySize);
          import_blob.appendString(public_key.c_str());
          return import_blob;
        };
      }
      default:
        return iroha::expected::makeError("Unsupported public key type.");
    }
  }

  iroha::expected::Result<cxi::Key, std::string> makeCxiKey(
      cxi::Cxi &cxi,
      iroha::multihash::Type type,
      PublicKeyHexStringView public_key,
      std::string const &temporary_key_name,
      std::optional<std::string> const &temporary_key_group) {
    return makeCxiKeyImportBlob(type, public_key) | [&](auto const &import_blob)
               -> iroha::expected::Result<cxi::Key, std::string> {
      cxi::PropertyList key_descr;
      key_descr.setName(temporary_key_name.c_str());
      if (temporary_key_group) {
        key_descr.setGroup(temporary_key_group->c_str());
      }
      try {
        return cxi.key_import(CXI_KEY_FLAG_VOLATILE | CXI_KEY_FLAG_OVERWRITE,
                              CXI_KEY_BLOB_SIMPLE,
                              key_descr,
                              import_blob,
                              nullptr);
      } catch (const cxi::Exception &e) {
        return iroha::expected::makeError(
            fmt::format("Could not prepare puclic key: {}", e));
      }
    };
  }
}  // namespace

CryptoVerifier::CryptoVerifier(std::shared_ptr<Connection> connection,
                               std::string temporary_key_name,
                               std::optional<std::string> temporary_key_group)
    : connection_holder_(std::move(connection)),
      connection_(*connection_holder_),
      temporary_key_name_(std::move(temporary_key_name)),
      temporary_key_group_(std::move(temporary_key_group)) {}

iroha::expected::Result<void, const char *> CryptoVerifier::verify(
    SignedHexStringView signature,
    ByteRange source,
    iroha::multihash::Type type,
    PublicKeyHexStringView public_key) const {
  std::lock_guard<std::mutex> lock{connection_.mutex};

  cxi::Cxi &cxi = *connection_.cxi;

  return makeCxiKey(
             cxi, type, public_key, temporary_key_name_, temporary_key_group_)
      | [&source, &signature](cxi::Key const &key) {
          return iroha::hexstringToBytestringResult(signature) |
              [&source, &key](cxi::ByteArray signature) {
                cxi::ByteArray message = irohaToCxiBuffer(source);

                MechanismParameter mech;
                auto cxi_algo = multihashEd25519ToCxiHashAlgo(type);
                if (not cxi_algo) {
                  return iroha::expected::makeError(
                      "Unsupported signature type.");
                }
                mech.set(cxi_algo.value());

                bool verification_successful = false;
                try {
                  verification_successful =
                      cxi.verify(CXI_FLAG_HASH_DATA | CXI_FLAG_CRYPT_FINAL,
                                 key,
                                 mech,
                                 message,
                                 &signature,
                                 nullptr);
                } catch (const cxi::Exception &e) {
                  return iroha::expected::makeError(
                      "Signature verification failed: {}", e);
                }

                if (verification_successful) {
                  return iroha::expected::Value<void>{};
                }
                return iroha::expected::makeError("Wrong signature.");
              };
        };
}

std::vector<iroha::multihash::Type> CryptoVerifier::getSupportedTypes() const {
  return {};
}
