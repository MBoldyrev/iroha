/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "cryptography/hsm_utimaco/crypto_signer.hpp"

#include <fmt/format.h>
#include "common/to_string.hpp"
#include "cryptography/blob.hpp"
#include "cryptography/crypto_provider/crypto_signer.hpp"
#include "cryptography/keypair.hpp"

using namespace shared_model::crypto;
using namespace shared_model::crypto::hsm_utimaco;

CryptoSignerUtimaco::CryptoSignerUtimaco(std::shared_ptr<Connection> connection)
    : connection_(std::move(connection)) {}

CryptoSignerUtimaco::~CryptoSignerUtimaco() = default;

std::string CryptoSignerUtimaco::sign(const Blob &blob) const {
  return Algorithm::sign(blob, keypair_);
}

shared_model::interface::types::PublicKeyHexStringView
CryptoSignerUtimaco::publicKey() const {
  return keypair_.publicKey();
}

std::string CryptoSignerUtimaco::toString() const {
  return fmt::format("Internal cryptographic signer of {}, {}",
                     Algorithm::kName,
                     iroha::to_string::toString(publicKey()));
}
