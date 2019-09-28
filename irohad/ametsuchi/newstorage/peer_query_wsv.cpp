/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ametsuchi/newstorage/peer_query_wsv.hpp"

#include <numeric>

#include "ametsuchi/wsv_query.hpp"

namespace iroha {
  namespace newstorage {

    PeerQueryWsv::PeerQueryWsv(std::shared_ptr<ametsuchi::WsvQuery> wsv)
        : wsv_(std::move(wsv)) {}

    boost::optional<std::vector<ametsuchi::PeerQuery::wPeer>>
    PeerQueryWsv::getLedgerPeers() {
      return wsv_->getPeers();
    }

  }  // namespace ametsuchi
}  // namespace iroha
