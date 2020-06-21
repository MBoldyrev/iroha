/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "cryptography/hsm_utimaco/crypto_signer.hpp"

#include <fmt/format.h>
#include "CXI/include/cxi.h"
#include "cryptography/blob.hpp"
#include "cryptography/hsm_utimaco/common.hpp"
#include "multihash/multihash.hpp"

using namespace shared_model::crypto;
using namespace shared_model::crypto::hsm_utimaco;

CryptoSignerUtimaco::CryptoSignerUtimaco(std::shared_ptr<Connection> connection,
                                         std::unique_ptr<cxi::Key> key,
                                         iroha::multihash::Type multihash_type,
                                         int cxi_algo)
    : connection_holder_(std::move(connection)),
      connection_(*connection_holder_),
      key_(std::move(key)),
      public_key_(iroha::multihash::encode(multihash_type,
                                           key_->getPublicKey().toString())),
      cxi_algo_(cxi_algo) {
  assert(multihashEd25519ToCxiHashAlgo(multihash_type) == cxi_algo_);
}

CryptoSignerUtimaco::~CryptoSignerUtimaco() = default;

std::string CryptoSignerUtimaco::sign(const Blob &blob) const {
  std::lock_guard<std::mutex> lock{connection_.mutex};

  cxi::MechanismParameter mech;
  mech.set(cxi_algo_);

  cxi::ByteArray result =
      connection_.cxi->sign(CXI_FLAG_HASH_DATA | CXI_FLAG_CRYPT_FINAL,
                            *key_,
                            mech,
                            irohaToCxiBuffer(blob.range()),
                            nullptr);

  return std::move(result).toString();
}

shared_model::interface::types::PublicKeyHexStringView
CryptoSignerUtimaco::publicKey() const {
  return shared_model::interface::types::PublicKeyHexStringView{public_key_};
}

std::string CryptoSignerUtimaco::toString() const {
  return fmt::format("HSM Utimaco cryptographic signer with public key '{}'",
                     public_key_);
}
