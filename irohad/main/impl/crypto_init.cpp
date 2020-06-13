/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "main/impl/crypto_init.hpp"

#include "common/result.hpp"
#include "crypto/keys_manager_impl.hpp"
#include "cryptography/crypto_provider/crypto_provider.hpp"
#include "cryptography/crypto_provider/crypto_signer_internal.hpp"
#include "cryptography/crypto_provider/crypto_verifier.hpp"
#include "cryptography/ed25519_sha3_impl/crypto_provider.hpp"
#include "interfaces/common_objects/string_view_types.hpp"
#include "main/iroha_conf_literals.hpp"
#include "main/iroha_conf_loader.hpp"
#include "multihash/multihash.hpp"
#include "multihash/type.hpp"

#if defined(USE_LIBURSA)
#include "cryptography/ed25519_ursa_impl/crypto_provider.hpp"
#define ED25519_PROVIDER CryptoProviderEd25519Ursa
#endif

namespace {
  const std::string kDefaultCryptoSignerTag{
      config_members::kCryptoProviderDefault};
  const std::string kDefaultCryptoVerifierTag{
      config_members::kCryptoProviderDefault};

  void checkCrypto(
      shared_model::crypto::CryptoProvider const &crypto_provider) {
    shared_model::crypto::Blob test_blob{"12345"};
    auto signature = crypto_provider.signer->sign(test_blob);
    if (auto e = iroha::expected::resultToOptionalError(
            crypto_provider.verifier->verify(
                shared_model::interface::types::SignedHexStringView{signature},
                test_blob,
                crypto_provider.signer->publicKey()))) {
      throw iroha::InitCryptoProviderException{
          fmt::format("Cryptography startup check failed: {}.", e.value())};
    }
  }

  std::unique_ptr<shared_model::crypto::CryptoSigner>
  makeCryptoSignerInternal() {
    using namespace shared_model::crypto;
    using namespace shared_model::interface::types;
    using SignerOrError =
        iroha::expected::Result<std::unique_ptr<CryptoSigner>, std::string>;
    iroha::KeysManagerImpl keys_manager(
        FLAGS_keypair_name, log_manager->getChild("KeysManager")->getLogger());
    SignerOrError signer_result;
    signer_result =
        (FLAGS_keypair_name.empty()
             ? iroha::expected::makeError("please specify --keypair_name to "
                                          "use internal crypto signer")
             : iroha::KeysManagerImpl{FLAGS_keypair_name,
                                      log_manager->getChild("KeysManager")
                                          ->getLogger()}
                   .loadKeys(boost::none))
        |
        [&](auto &&keypair) {
          return iroha::hexstringToBytestringResult(keypair.publicKey()) |
                     [&keypair](auto const &public_key) -> SignerOrError {
            using DefaultSigner =
                shared_model::crypto::CryptoProviderEd25519Sha3;
            if (public_key.size() == DefaultSigner::kPublicKeyLength) {
              return std::make_unique<CryptoSignerInternal<DefaultSigner>>(
                  std::move(keypair));
            }
            return iroha::multihash::createFromBuffer(makeByteRange(public_key))
                       |
                       [&keypair](const iroha::multihash::Multihash &public_key)
                       -> SignerOrError {
              // prevent unused warnings when compiling without any additional
              // crypto engines:
              (void)keypair;

              using iroha::multihash::Type;
              switch (public_key.type) {
#if defined(ED25519_PROVIDER)
                case Type::ed25519pub:
                  return std::make_unique<
                      CryptoSignerInternal<ED25519_PROVIDER>>(
                      std::move(keypair));
#endif
                default:
                  return iroha::expected::makeError(
                      "Unknown crypto algorithm.");
              };
            };
          };
        };
    if (auto e = iroha::expected::resultToOptionalError(signer_result)) {
      throw iroha::InitCryptoProviderException{
          fmt::format("Failed to load keypair: {}", e.value())};
    }
    return std::move(signer_result).assumeValue();
  }

  std::unique_ptr<shared_model::crypto::CryptoVerifier>
  makeCryptoVerifierInternal() {
    return std::make_unique<shared_model::crypto::CryptoVerifier>();
  }

  void makeHsmUtimacoCriptoProvider(
      IrohadConfig::Crypto::HsmUtimaco connection_param,
      std::optional<std::reference_wrapper<
          std::shared_ptr<shared_model::crypto::CryptoSigner>>> signer,
      std::optional<std::reference_wrapper<
          std::shared_ptr<shared_model::crypto::CryptoVerifier>>> verifier) {}
}  // namespace

namespace iroha {
  shared_model::crypto::CryptoProvider makeCryptoProvider(
      IrohadConfig::Crypto const &config) {
    using namespace shared_model::crypto;
    CryptoProvider crypto_provider;

    struct AlgorithmInitializer {
      IrohadConfig::Crypto::ProviderVariant connection_params;
      std::optional<std::reference_wrapper<std::shared_ptr<CryptoSigner>>>
          signer;
      std::optional<std::reference_wrapper<std::shared_ptr<CryptoVerifier>>>
          verifier;
    };

    std::unordered_map<IrohadConfig::Crypto::ProviderId, AlgorithmInitializer>
        initializers;

    auto get_provider_conf_param =
        [&config](IrohadConfig::Crypto::ProviderId tag)
        -> IrohadConfig::Crypto::ProviderVariant const & {
      if (tag == config_members::kCryptoProviderDefault) {
        static const IrohadConfig::Crypto::Default kDefaultParam;
        return kDefaultParam;
      }
      const auto conf_it = config.providers.find(config.signer);
      if (conf_it == config.providers.end()) {
        throw InitCryptoProviderException{fmt::format(
            "Crypto provider with tag '{}' requested but not defined",
            config.signer)};
      }
      return conf_it->second;
    };

    auto get_initializer =
        [&initializers, &get_provider_conf_param](
            IrohadConfig::Crypto::ProviderId tag) -> AlgorithmInitializer & {
      auto init_it = initializers.find(tag);
      if (init_it == initializers.end()) {
        init_it =
            initializers
                .emplace(tag,
                         AlgorithmInitializer{get_provider_conf_param(tag),
                                              std::nullopt,
                                              std::nullopt})
                .first;
      }
      return init_it->second;
    };

    get_initializer(config.signer).signer = crypto_provider.signer;
    get_initializer(config.verifier).verifier = crypto_provider.verifier;

    for (auto const &pair : initializers) {
      auto &initializer = pair.second;
      std::visit(
          iroha::make_visitor(
              [&initializer](IrohadConfig::Crypto::Default const &) {
                if (initializer.signer) {
                  initializer.signer->get() = makeCryptoSignerInternal();
                }
                if (initializer.verifier) {
                  initializer.verifier->get() = makeCryptoVerifierInternal();
                }
              },
              [&initializer](IrohadConfig::Crypto::HsmUtimaco const &param) {
                makeHsmUtimacoCriptoProvider(
                    param, initializer.signer, initializer.verifier);
              }),
          initializer.connection_params);
    }

    assert(crypto_provider.signer);
    assert(crypto_provider.verifier);
    checkCrypto(crypto_provider);
    return crypto_provider;
  }
}  // namespace iroha
