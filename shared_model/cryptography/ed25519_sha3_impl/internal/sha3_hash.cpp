/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "cryptography/ed25519_sha3_impl/internal/sha3_hash.hpp"

#include <ed25519_cpp/sha256.hpp>
#include <ed25519_cpp/sha512.hpp>

namespace iroha {

  void sha3_256(uint8_t *output, const uint8_t *input, size_t in_size) {
    using namespace iroha_ed25519::v2;
    iroha_ed25519::v2::sha256(Sha256{output}, MessageView{input, in_size});
  }

  void sha3_512(uint8_t *output, const uint8_t *input, size_t in_size) {
    using namespace iroha_ed25519::v2;
    iroha_ed25519::v2::sha512(Sha512{output}, MessageView{input, in_size});
  }

  hash256_t sha3_256(shared_model::interface::types::ByteRange input) {
    using namespace iroha_ed25519::v2;
    hash256_t hash;
    iroha_ed25519::v2::sha256(Sha256{hash.data()}, MessageView{input});
    return hash;
  }

  hash512_t sha3_512(shared_model::interface::types::ByteRange input) {
    using namespace iroha_ed25519::v2;
    hash512_t hash;
    iroha_ed25519::v2::sha512(Sha512{hash.data()}, MessageView{input});
    return hash;
  }

  hash512_t sha3_512(const uint8_t *input, size_t in_size) {
    using namespace iroha_ed25519::v2;
    hash512_t hash;
    iroha_ed25519::v2::sha512(Sha512{hash.data()}, MessageView{input, in_size});
    return hash;
  }

  hash256_t sha3_256(const std::string &msg) {
    using namespace iroha_ed25519::v2;
    hash256_t hash;
    iroha_ed25519::v2::sha256(Sha256{hash.data()}, MessageView{msg});
    return hash;
  }

  hash512_t sha3_512(const std::string &msg) {
    hash512_t h;
    sha3_512(
        h.data(), reinterpret_cast<const uint8_t *>(msg.data()), msg.size());
    return h;
  }

  hash512_t sha3_512(const std::vector<uint8_t> &msg) {
    hash512_t h;
    sha3_512(h.data(), msg.data(), msg.size());
    return h;
  }

  hash256_t sha3_256(const std::vector<uint8_t> &msg) {
    using namespace iroha_ed25519::v2;
    hash256_t hash;
    iroha_ed25519::v2::sha256(Sha256{hash.data()}, MessageView{msg});
    return hash;
  }
}  // namespace iroha
