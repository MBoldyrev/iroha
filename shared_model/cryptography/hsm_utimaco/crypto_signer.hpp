/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_CRYPTO_SIGNER_UTIMACO_HPP
#define IROHA_CRYPTO_SIGNER_UTIMACL_HPP

#include "cryptography/crypto_provider/crypto_signer.hpp"

#include <memory>

#include "cryptography/hsm_utimaco/connection.hpp"

namespace shared_model {
  namespace crypto {
    namespace hsm_utimaco {
      /**
       * CryptoSignerUtimaco - wrapper for Utimaco HSM crypto singing
       */
      class CryptoSignerUtimaco : public CryptoSigner {
       public:
        explicit CryptoSignerUtimaco(std::shared_ptr<Connection> connection);

        virtual ~CryptoSignerUtimaco();

        std::string sign(const Blob &blob) const override;

        shared_model::interface::types::PublicKeyHexStringView publicKey()
            const override;

        std::string toString() const override;

       private:
        std::shared_ptr<Connection> connection_;
      };
    }  // namespace hsm_utimaco
  }    // namespace crypto
}  // namespace shared_model

#endif
