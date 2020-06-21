/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_HSM_UTIMACO_CRYPTO_VERIFIER_HPP
#define IROHA_HSM_UTIMACO_CRYPTO_VERIFIER_HPP

#include "cryptography/crypto_provider/crypto_verifier_multihash.hpp"

#include <optional>
#include <string>

#include "cryptography/hsm_utimaco/connection.hpp"

namespace shared_model {
  namespace crypto {
    namespace hsm_utimaco {

      /**
       * CryptoVerifier - verifier
       * cryptographic signatures
       */
      class CryptoVerifier
          : public shared_model::crypto::CryptoVerifierMultihash {
       public:
        explicit CryptoVerifier(std::shared_ptr<Connection> connection);

        iroha::expected::Result<void, const char *> verify(
            shared_model::interface::types::SignedHexStringView signature,
            shared_model::interface::types::ByteRange source,
            iroha::multihash::Type type,
            shared_model::interface::types::PublicKeyHexStringView public_key)
            const override;

        std::vector<iroha::multihash::Type> getSupportedTypes() const override;

       private:
        std::shared_ptr<Connection> connection_holder_;
        Connection &connection_;

        std::string temporary_key_name_;
        std::optional<std::string> temporary_key_group_;
      };

    }  // namespace hsm_utimaco
  }    // namespace crypto
}  // namespace shared_model

#endif  // IROHA_CRYPTO_VERIFIER_HPP
