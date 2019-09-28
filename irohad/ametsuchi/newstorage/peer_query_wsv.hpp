/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_NEWSTORAGE_PEER_QUERY_WSV_HPP
#define IROHA_NEWSTORAGE_PEER_QUERY_WSV_HPP

#include "ametsuchi/peer_query.hpp"

#include <memory>
#include <vector>

namespace iroha {
  namespace ametsuchi {
    class WsvQuery;
  }

  namespace newstorage {
    /**
     * Implementation of PeerQuery interface based on WsvQuery fetching
     */
   class PeerQueryWsv : public ametsuchi::PeerQuery {
     public:
      explicit PeerQueryWsv(std::shared_ptr<ametsuchi::WsvQuery> wsv);

      /**
       * Fetch peers stored in ledger
       * @return list of peers in insertion to ledger order
       */
      boost::optional<std::vector<wPeer>> getLedgerPeers() override;

     private:
      std::shared_ptr<ametsuchi::WsvQuery> wsv_;
    };

  }  // namespace newstorage
}  // namespace iroha
#endif  // IROHA_NEWSTORAGE_PEER_QUERY_WSV_HPP
