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
      auto res = wsv_.getPeers(
          std::string(),
          [&peers, this](const std::string& pk, const std::string& address) {
            auto blob = shared_model::crypto::Blob::fromHexString(pk);
            if (blob.size() > 0 && !address.empty()) {
              peers.emplace_back(
                  std::make_shared<shared_model::plain::Peer>(
                      address,
                      shared_model::crypto::PublicKey{
                          shared_model::crypto::Blob::fromHexString(pk)}
                  )
              );
            } else {
              log_->error("invalid peer pk={}, address={}", pk, address);
            }
          }
      );
      if (res == ResultCode::kOk) {
        return boost::make_optional(std::move(peers));
      }
      // TODO res to str -> log
      return boost::none;
    }
  }  // namespace newstorage
}  // namespace iroha
