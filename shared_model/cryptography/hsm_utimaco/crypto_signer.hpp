/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_CRYPTO_SIGNER_UTIMACO_HPP
#define IROHA_CRYPTO_SIGNER_UTIMACL_HPP

#include "cryptography/crypto_provider/crypto_signer.hpp"

#include <memory>

#include "cryptography/hsm_utimaco/connection.hpp"

namespace cxi {
  class Key;
}

namespace shared_model {
  namespace crypto {
    namespace hsm_utimaco {
      /**
       * CryptoSignerUtimaco - wrapper for Utimaco HSM crypto singing
       */
      class CryptoSignerUtimaco : public CryptoSigner {
       public:
        CryptoSignerUtimaco(std::shared_ptr<Connection> connection,
                            std::unique_ptr<cxi::Key> key);

        virtual ~CryptoSignerUtimaco();

        std::string sign(const shared_model::crypto::Blob &blob) const override;

        shared_model::interface::types::PublicKeyHexStringView publicKey()
            const override;

        std::string toString() const override;

       private:
        std::shared_ptr<Connection> connection_holder_;
        Connection &connection_;
        std::unique_ptr<cxi::Key> key_;
        std::string public_key_;
        int cxi_algo_;
      };
    }  // namespace hsm_utimaco
  }    // namespace crypto
}  // namespace shared_model

#endif
