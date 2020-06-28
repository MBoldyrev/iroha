/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "cryptography/ed25519_sha3_impl/verifier.hpp"

#include <ed25519_cpp/ed25519.hpp>
#include <ed25519_cpp/sha256.hpp>
#include "common/hexutils.hpp"
#include "common/result.hpp"

using namespace shared_model::crypto::ed25519_sha3;
using namespace shared_model::interface::types;

Verifier::~Verifier() = default;

iroha::expected::Result<void, std::string> Verifier::verify(
    iroha::multihash::Type type,
    SignatureByteRangeView signature,
    ByteRange source,
    PublicKeyByteRangeView public_key) const {
  assert(type == iroha::multihash::Type::kEd25519Sha3_256);
  if (verifyEd25519Sha3(signature, source, public_key)) {
    return iroha::expected::Value<void>{};
  }
  return iroha::expected::makeError("Bad signature.");
}

bool Verifier::verifyEd25519Sha3(SignatureByteRangeView signature,
                                 ByteRange source,
                                 PublicKeyByteRangeView public_key) {
  using namespace iroha_ed25519::v2;
  Sha256 message_hash;
  if (not sha256(message_hash, MessageView{source})) {
    return false;
  }
  return iroha_ed25519::v2::verify(SignatureView{ByteRange{signature}},
                                   MessageView{message_hash},
                                   PublicKeyView{ByteRange{public_key}});
}

std::vector<iroha::multihash::Type> Verifier::getSupportedTypes() const {
  return {iroha::multihash::Type::kEd25519Sha3_256};
}
