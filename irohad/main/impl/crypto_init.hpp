/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_CRYPTO_INIT_HPP
#define IROHA_CRYPTO_INIT_HPP

#include <stdexcept>

#include "cryptography/crypto_provider/crypto_provider.hpp"
#include "main/iroha_conf_loader.hpp"

namespace iroha {
  class InitCryptoProviderException : public std::runtime_error {
    using std::runtime_error::runtime_error;
  };

  /**
   * init crypto
   * @param config crypto config
   * @return crypto provider
   * throws InitCryptoProviderException on failure
   */
  shared_model::crypto::CryptoProvider makeCryptoProvider(
      IrohadConfig::Crypto const &config);
}  // namespace iroha

#endif
