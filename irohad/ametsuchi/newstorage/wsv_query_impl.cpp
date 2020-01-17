/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "wsv_query_impl.hpp"
#include "immutable_wsv.hpp"
#include "backend/plain/peer.hpp"
#include "common/result.hpp"
#include "cryptography/public_key.hpp"
#include "logger/logger.hpp"

namespace iroha {
  namespace newstorage {

    using shared_model::interface::types::AccountIdType;
    using shared_model::interface::types::AddressType;
    using shared_model::interface::types::PubkeyType;

    WsvQueryImpl::WsvQueryImpl(ImmutableWsv &wsv, logger::LoggerPtr log)
        : wsv_(wsv), log_(std::move(log)) {}

    boost::optional<std::vector<PubkeyType>> WsvQueryImpl::getSignatories(
        const AccountIdType &account_id
    ) {
      std::vector<PubkeyType> signatories;
      auto res = wsv_.getSignatories(
          std::string(), account_id,
          [&signatories, this](const std::string& pk) {
            auto blob = shared_model::crypto::Blob::fromHexString(pk);
            if (blob.size() > 0) {
              signatories.emplace_back(blob);
            } else {
              log_->error("invalid public key {}", pk);
            }
          }
      );
      if (res == ResultCode::kOk) {
        return boost::make_optional(std::move(signatories));
      }
      // TODO res to str -> log
      return boost::none;
    }

    boost::optional<std::vector<std::shared_ptr<shared_model::interface::Peer>>>
    WsvQueryImpl::getPeers() {
      std::vector<std::shared_ptr<shared_model::interface::Peer>> peers;
      auto res = wsv_.getPeers(std::string(), [&peers, this](PeerView peer) {
        auto blob = shared_model::crypto::Blob::fromHexString(peer.pub_key);
        if (blob.size() > 0 && !peer.address.empty()) {
          peers.emplace_back(std::make_shared<shared_model::plain::Peer>(
              peer.address,
              shared_model::crypto::PublicKey{
                  shared_model::crypto::Blob::fromHexString(peer.pub_key)},
              peer.tls_certificate));
        } else {
          log_->error(
              "invalid peer pk={}, address={}", peer.pub_key, peer.address);
        }
      });
      if (res == ResultCode::kOk) {
        if (not peers.empty()) {
          return boost::make_optional(std::move(peers));
        }
        return boost::none;
      }
      // TODO res to str -> log
      return boost::none;
    }

    boost::optional<std::shared_ptr<shared_model::interface::Peer>>
    WsvQueryImpl::getPeerByPublicKey(
        const shared_model::interface::types::PubkeyType &public_key) {
      boost::optional<std::shared_ptr<shared_model::interface::Peer>> peer;
      auto res =
          wsv_.getPeerByPublicKey({}, public_key.hex(), peer);
      if (res == ResultCode::kOk) {
        return peer;
      }
      // TODO res to str -> log
      return boost::none;
    }
  }  // namespace newstorage
}  // namespace iroha
