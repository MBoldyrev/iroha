/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_PROTO_ADD_PEER_HPP
#define IROHA_PROTO_ADD_PEER_HPP

#include "interfaces/commands/add_peer.hpp"

#include "backend/protobuf/common_objects/peer.hpp"
#include "commands.pb.h"
#include "interfaces/common_objects/peer.hpp"

namespace shared_model {

  class AddPeer final : public AddPeer {
   public:
    explicit AddPeer(iroha::protocol::Command &command);

    const Peer &peer() const override;

   private:
    const iroha::protocol::AddPeer &add_peer_;
    Peer peer_;
  };
}  // namespace shared_model

#endif  // IROHA_PROTO_ADD_PEER_HPP
