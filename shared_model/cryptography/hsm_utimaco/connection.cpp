/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "cryptography/hsm_utimaco/connection.hpp"

#include <cstdio>
#include <memory>
#include <mutex>
#include <tuple>
#include <unordered_map>

#include <boost/range/adaptor/map.hpp>
#include <vector>
#include "cxi.h"

using namespace shared_model::crypto::hsm_utimaco;

namespace {
  constexpr int kActionTimeoutMs{5000};
  constexpr int kConnectTimeoutMs{10000};
  constexpr size_t kTransportBufferSizeBytes = 10 * 1024;

  std::unordered_map<std::string, cxi::Log::levels> const &getLogLevelsMap() {
    static std::unordered_map<std::string, cxi::Log::levels> map{
        {"none", cxi::Log::LEVEL_NONE},
        {"error", cxi::Log::LEVEL_ERROR},
        {"warning", cxi::Log::LEVEL_WARNING},
        {"info", cxi::Log::LEVEL_INFO},
        {"trace", cxi::Log::LEVEL_TRACE},
        {"debug", cxi::Log::LEVEL_DEBUG}};
  }

  inline cxi::ByteArray irohaToCxiBuffer(
      shared_model::interface::types::ByteRange range) {
    static_assert(sizeof(*range.data()) == sizeof(char), "type mismatch");
    return cxi::ByteArray{reinterpret_cast<char const *>(range.data()),
                          static_cast<int>(range.length())};
  }

  /*
  std::string cxiBufToString(cxi::ByteArray const& cxi_buf) {
  }
  */
}  // namespace

struct Connection::Impl {
  std::unique_ptr<cxi::Cxi> cxi;
  std::mutex mu;

  std::unordered_map<std::tuple<std::string, std::optional<std::string>>,
                     cxi::Key>
      keys;

  // throws cxi::Exception
  cxi::Key &getKeyCached(
      IrohadConfig::Crypto::HsmUtimaco::KeyHandle const &handle) {
    auto const id = std::make_tuple(handle.name, handle.group);
    auto it = keys.find(id);
    if (it != keys.end()) {
      return it->second;
    }

    // open the key
    cxi::PropertyList key_descr;
    key_descr.setName(handle.name.c_str());
    if (handle.group) {
      key_descr.setGroup(handle.group->c_str());
    }
    auto const emplace_result = keys.emplace(id, cxi->key_open(0, key_descr));
    return emplace_result.second->second;
  }
};

Connection::Connection(IrohadConfig::Crypto::HsmUtimaco const &config)
    : impl_(std::make_unique<Impl>()) {
  if (config.log) {
    auto const &log_config = config.log.value();
    auto level_it = getLogLevelsMap().find(log_config.level);
    if (level_it == getLogLevelsMap().end()) {
      throw InitCryptoProviderException {
        fmt::format(
            "Unknown log level specified. Allowed values are: '{}'.",
            fmt::join(getLogLevelsMap() | boost::range::adaptors::map_keys(),
                      "', '"));
      };
    }
    cxi::Log::getInstance().init(log_config.path.c_str(), level_it->second);
  }

  std::vector<char const *> devices_raw;
  devices_raw.reserve(config.devices.size());
  for (auto const &device : config.devices) {
    devices_raw.emplace_back(devices_raw.c_str());
  }
  impl_->cxi = std::make_unique<cxi::Cxi>(devices_raw.data(),
                                          devices_raw.size(),
                                          kActionTimeoutMs,
                                          kConnectTimeoutMs);
}

Connection::~Connection() = default;

std::string Connection::sign(
    const Blob &blob,
    IrohadConfig::Crypto::HsmUtimaco::KeyHandle const &key_handle) const {
  std::lock_guard<std::mutex> lock{impl_->mu};

  cxi::Key &key = impl_->getKeyCached(key_handle);

  cxi::MechanismParameter mech;
  mech.set(CXI_MECH_HASH_ALGO_SHA512);  // set all params explicitly

  cxi::ByteArray result =
      impl_->cxi->sign(CXI_FLAG_HASH_DATA | CXI_FLAG_CRYPT_FINAL,
                       key,
                       mech,
                       cxi::ByteArray{buf, read_sz},
                       nullptr);

  return std::move(result).toString();
}

std::string Connection::publicKey(
    IrohadConfig::Crypto::HsmUtimaco::KeyHandle const &key) const {
  std::lock_guard<std::mutex> lock{impl_->mu};

  cxi::Key &key = impl_->getKeyCached(key_handle);

  return key.getPublicKey().toString();
}

bool Connection::verify(
    shared_model::interface::types::SignedHexStringView signature,
    const Blob &source,
    shared_model::interface::types::PublicKeyHexStringView public_key) const {
  std::lock_guard<std::mutex> lock{impl_->mu};

  cxi::PropertyList key_descr;
  key_descr.setName(kVerifyingKeyName);
  if (kVerifyingKeyGroup) {
    key_descr.setGroup(kVerifyingKeyGroup);
  }
  key = cxi->key_open(0, key_descr);

  cxi::ByteArray data{irohaToCxiBuffer(source.range())};
  verification_result = cxi.verify(CXI_FLAG_HASH_DATA | CXI_FLAG_CRYPT_FINAL,
                                   key,
                                   mech,
                                   data,
                                   &signature,
                                   &iv);
}

std::string Connection::toString() const override;
