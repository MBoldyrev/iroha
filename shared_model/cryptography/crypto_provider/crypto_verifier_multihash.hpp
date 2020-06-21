/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_CRYPTO_VERIFIER_HPP
#define IROHA_CRYPTO_VERIFIER_HPP

#include <vector>

#include "common/result_fwd.hpp"
#include "interfaces/common_objects/string_view_types.hpp"
#include "multihash/type.hpp"

namespace shared_model {
  namespace crypto {

    /**
     * CryptoVerifier - adapter for generalization verification of cryptographic
     * signatures
     * @tparam Algorithm - cryptographic algorithm for verification
     */
    class CryptoVerifierMultihash {
     public:
      /**
       * Verify signature attached to source data
       * @param signedData - cryptographic signature
       * @param source - data that was signed
       * @param pubKey - public key of signatory
       * @return a result of void if signature is correct or error message
       * otherwise or if verification could not be completed
       */
      virtual iroha::expected::Result<void, const char *> verify(
          shared_model::interface::types::SignedHexStringView signature,
          shared_model::interface::types::ByteRange source,
          iroha::multihash::Type type,
          shared_model::interface::types::PublicKeyHexStringView public_key)
          const = 0;

      virtual std::vector<iroha::multihash::Type> getSupportedTypes() const = 0;
    };
  }  // namespace crypto
}  // namespace shared_model

#endif  // IROHA_CRYPTO_VERIFIER_HPP
