/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "cryptography/hsm_utimaco/init.hpp"

#include <unordered_map>

#include <boost/range/adaptor/map.hpp>
#include "CXI/include/cxi.h"

namespace {
  constexpr int kActionTimeoutMs{5000};
  constexpr int kConnectTimeoutMs{10000};
  constexpr size_t kTransportBufferSizeBytes = 10 * 1024;

  // throws InitCryptoProviderException
  cxi::Log::levels const &getCxiLogLevel(std::string const &level) {
    static std::unordered_map<std::string, cxi::Log::levels> map{
        {"none", cxi::Log::LEVEL_NONE},
        {"error", cxi::Log::LEVEL_ERROR},
        {"warning", cxi::Log::LEVEL_WARNING},
        {"info", cxi::Log::LEVEL_INFO},
        {"trace", cxi::Log::LEVEL_TRACE},
        {"debug", cxi::Log::LEVEL_DEBUG}};
    auto it = map.find(level);
    if (it == map.end()) {
      throw InitCryptoProviderException {
        fmt::format(
            "Unknown log level specified. Allowed values are: '{}'.",
            fmt::join(map | boost::range::adaptors::map_keys(), "', '"));
      };
    }
    return it->second;
  }

  // throws InitCryptoProviderException
  iroha::multihash::Type const &getMultihashType(std::string const &type) {
    using iroha::multihash::Type;
    // types supported by Utimaco
    static std::unordered_map<std::string, Type> map{
        {"ECDSA_SHA2_224", Type::kEcdsaSha2_224},
        {"ECDSA_SHA2_256", Type::kEcdsaSha2_256},
        {"ECDSA_SHA2_384", Type::kEcdsaSha2_384},
        {"ECDSA_SHA2_512", Type::kEcdsaSha2_512},
        {"ECDSA_SHA3_224", Type::kEcdsaSha3_224},
        {"ECDSA_SHA3_256", Type::kEcdsaSha3_256},
        {"ECDSA_SHA3_384", Type::kEcdsaSha3_384},
        {"ECDSA_SHA3_512", Type::kEcdsaSha3_512}};
    auto it = map.find(type);
    if (it == map.end()) {
      throw InitCryptoProviderException {
        fmt::format(
            "Unknown signature type specified. Allowed values are: '{}'.",
            fmt::join(map | boost::range::adaptors::map_keys(), "', '"));
      };
    }
    return it->second;
  }

  // throws InitCryptoProviderException
  std::unique_ptr<Connection> makeConnection(
      IrohadConfig::Crypto::HsmUtimaco const &config) {
    std::vector<char const *> devices_raw;
    devices_raw.reserve(config.devices.size());
    for (auto const &device : config.devices) {
      devices_raw.emplace_back(devices_raw.c_str());
    }

    auto connection = std::make_unique<Connection>();
    connection->cxi = std::make_unique<cxi::Cxi>(devices_raw.data(),
                                                 devices_raw.size(),
                                                 kActionTimeoutMs,
                                                 kConnectTimeoutMs);
    return connection;
  }

  // throws InitCryptoProviderException
  std::unique_ptr<shared_model::crypto::CryptoSigner> makeSigner(
      IrohadConfig::Crypto::HsmUtimaco const &config,
      std::shared_ptr<Connection> connection) {
    auto const &signer_config = config.signer.value();

    // get the signature type
    iroha::multihash::Type multihash_type = signer_config.type;
    auto cxi_algo = multihashEd25519ToCxiHashAlgo(type);
    if (not cxi_algo) {
      throw InitCryptoProviderException{"Unsupported signature type."};
    }

    // prepare the signing key
    cxi::PropertyList key_descr;
    auto const &key_id = signer_config.signing_key;
    key_descr.setName(key_id.name.c_str());
    if (key_id.group) {
      key_descr.setGroup(key_id.group->c_str());
    }
    std::unique_ptr<cxi::Key> key;
    try {
      key = std::make_unique<cxi::Key>(connection.cxi->key_open(0, key_descr));
    } catch (const cxi::Exception &e) {
      return iroha::expected::makeError(
          fmt::format("Could not open signing key: {}", e));
    }
    if (not key) {
      return iroha::expected::makeError("Could not open signing key.");
    }

    return std::make_unique<CryptoSignerUtimaco>(
        std::move(connection), std::move(key), multihash_type, cxi_algo);
  }
}  // namespace

void iroha::makeCryptoProviderUtimaco(
    iroha::PartialCryptoInit initializer,
    IrohadConfig::Crypto::HsmUtimaco const &config,
    logger::LoggerManagerTreePtr log_manager) {
  if (config.log) {
    cxi::Log::getInstance().init(config.log->path.c_str(),
                                 getCxiLogLevel(config.log->level));
  }

  std::shared_ptr<Connection> = makeConnection(config);

  if (initializer.signer) {
    initializer.signer->get() = makeSigner(config, connection);
  }

  initializer.verifiers.emplace_back(std::make_unique<CryptoVerifier>(
      connection, config.temporary_key.name, config.temporary_key.group));
}
