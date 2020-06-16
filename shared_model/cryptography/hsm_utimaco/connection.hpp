/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_CRYPTO_HSM_UTIMACO_CONNECTION_HPP
#define IROHA_CRYPTO_HSM_UTIMACL_CONNECTION_HPP

#include <memory>
#include <string>

//#include "cxi.h"
#include "interfaces/common_objects/string_view_types.hpp"
#include "main/iroha_conf_loader.hpp"

namespace shared_model {
  namespace crypto {
    namespace hsm_utimaco {
      /**
       * Connection - abstraction of connection to HSM Utimaco node.
       * Also performs authentication at startup.
       */
      class Connection {
       public:
        explicit Connection(IrohadConfig::Crypto::HsmUtimaco const &config);

        ~Connection();

        std::string sign(
            const Blob &blob,
            IrohadConfig::Crypto::HsmUtimaco::KeyHandle const &key) const;

        std::string publicKey(
            IrohadConfig::Crypto::HsmUtimaco::KeyHandle const &key) const;

        bool verify(
            shared_model::interface::types::SignedHexStringView signature,
            const Blob &source,
            shared_model::interface::types::PublicKeyHexStringView public_key)
            const;

        std::string toString() const override;

       private:
        struct Impl;
        std::unique_ptr<Impl> impl_;
      };
    }  // namespace hsm_utimaco
  }    // namespace crypto
}  // namespace shared_model

#endif
