/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_PROTO_PEERS_RESPONSE_HPP
#define IROHA_SHARED_MODEL_PROTO_PEERS_RESPONSE_HPP

#include "interfaces/query_responses/peers_response.hpp"

#include "backend/protobuf/common_objects/peer.hpp"
#include "qry_responses.pb.h"

namespace shared_model {
  class PeersResponse final : public PeersResponse {
   public:
    explicit PeersResponse(iroha::protocol::QueryResponse &query_response);

    PeersForwardCollectionType peers() const override;

   private:
    const iroha::protocol::PeersResponse &peers_response_;

    std::vector<Peer> peers_;
  };
}  // namespace shared_model

#endif  // IROHA_SHARED_MODEL_PROTO_PEERS_RESPONSE_HPP
