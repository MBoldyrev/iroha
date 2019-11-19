/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_PROTO_GET_PEERS_HPP
#define IROHA_PROTO_GET_PEERS_HPP

#include "interfaces/queries/get_peers.hpp"

#include "queries.pb.h"

namespace shared_model {
  class GetPeers final : public GetPeers {
   public:
    explicit GetPeers(iroha::protocol::Query &query);
  };

}  // namespace shared_model

#endif  // IROHA_PROTO_GET_PEERS_HPP
