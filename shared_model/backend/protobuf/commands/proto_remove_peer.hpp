/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_PROTO_REMOVE_PEER_HPP
#define IROHA_PROTO_REMOVE_PEER_HPP

#include "interfaces/commands/remove_peer.hpp"

#include "backend/protobuf/common_objects/peer.hpp"
#include "commands.pb.h"

namespace shared_model {

  class RemovePeer final : public RemovePeer {
   public:
    explicit RemovePeer(iroha::protocol::Command &command);

    const types::PubkeyType &pubkey() const override;

   private:
    const types::PubkeyType pubkey_;
  };
}  // namespace shared_model

#endif  // IROHA_PROTO_REMOVE_PEER_HPP
