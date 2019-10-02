/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "network/impl/channel_pool.hpp"

#include <shared_mutex>
#include <unordered_map>

#include "cryptography/blob_hasher.hpp"
#include "cryptography/public_key.hpp"
#include "interfaces/common_objects/peer.hpp"
#include "interfaces/common_objects/types.hpp"
#include "network/impl/channel_provider.hpp"

using namespace iroha::network;

class ChannelPool::Impl {
 public:
  Impl(std::unique_ptr<ChannelProvider> channel_provider)
      : channel_provider_(std::move(channel_provider)) {}

  std::shared_ptr<grpc::Channel> getOrCreate(
      const std::string &service_full_name,
      const shared_model::interface::Peer &peer) {
    std::shared_lock<std::shared_timed_mutex> read_lock(mutex_);
    auto i = channels_.find(peer.pubkey());
    if (i != channels_.end()) {
      return i->second;
    }
    read_lock.unlock();

    auto new_channel = channel_provider_->getChannel(service_full_name, peer);
    std::unique_lock<std::shared_timed_mutex> write_lock(mutex_);
    channels_[peer.pubkey()] = new_channel;
    return new_channel;
  }

 private:
  std::unique_ptr<ChannelProvider> channel_provider_;

  std::shared_timed_mutex mutex_;
  std::unordered_map<shared_model::crypto::PublicKey,
                     std::shared_ptr<grpc::Channel>,
                     shared_model::crypto::BlobHasher>
      channels_;
};

ChannelPool::ChannelPool(std::unique_ptr<ChannelProvider> channel_provider)
    : impl_(std::make_unique<Impl>(std::move(channel_provider))) {}

ChannelPool::~ChannelPool() = default;

std::shared_ptr<grpc::Channel> ChannelPool::getChannel(
    const std::string &service_full_name,
    const shared_model::interface::Peer &peer) {
  return impl_->getOrCreate(service_full_name, peer);
}
