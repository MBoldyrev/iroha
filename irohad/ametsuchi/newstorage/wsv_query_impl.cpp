/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "wsv_query_impl.hpp"
#include "rel_db_backend.hpp"
#include "backend/plain/peer.hpp"
#include "common/result.hpp"
#include "cryptography/public_key.hpp"
//#include "logger/logger.hpp"

namespace iroha {
  namespace newstorage {

    using shared_model::interface::types::AccountIdType;
    using shared_model::interface::types::AddressType;
    using shared_model::interface::types::PubkeyType;

    WsvQueryImpl::WsvQueryImpl(RelDbBackend &db, logger::LoggerPtr log)
        : db_(db), log_(std::move(log)) {}

    boost::optional<std::vector<PubkeyType>> WsvQueryImpl::getSignatories(
        const AccountIdType &account_id) {
        try {
            std::vector<PubkeyType> v;
            db_.getSignatories(
                account_id,
                [&v](const std::string& pk) {
                    // TODO errors
                    v.emplace_back(shared_model::crypto::Blob::fromHexString(pk));
                }
            );
            return boost::make_optional(std::move(v));

        } catch (const std::exception& e) {
            // log

        }
        return boost::none;
    }

    boost::optional<std::vector<std::shared_ptr<shared_model::interface::Peer>>>
    WsvQueryImpl::getPeers() {
        try {
            std::vector<std::shared_ptr<shared_model::interface::Peer>> v;
            db_.getPeers(
                [&v](const std::string& pk, const std::string& address) {
                    // TODO errors
                    v.emplace_back(
                        std::make_shared<shared_model::plain::Peer>(
                            address,
                            shared_model::crypto::PublicKey{
                                shared_model::crypto::Blob::fromHexString(pk)}
                        )
                    );
                }
            );
            return boost::make_optional(std::move(v));

        } catch (const std::exception& e) {
            // log

        }
        return boost::none;

    }
  }  // namespace newstorage
}  // namespace iroha
