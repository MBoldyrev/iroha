/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_REMOVE_PEER_HPP
#define IROHA_SHARED_MODEL_REMOVE_PEER_HPP

#include "interfaces/base/model_primitive.hpp"

#include "commands.pb.h"
#include "interfaces/common_objects/peer.hpp"

#include "interfaces/common_objects/peer.hpp"
#include "interfaces/common_objects/types.hpp"

namespace shared_model {

  /**
   * Remove a peer from Iroha
   */
  class RemovePeer : public ModelPrimitive<RemovePeer> {
   public:
    explicit RemovePeer(iroha::protocol::Command &command);

    /**
     * Return public key of peer to be removed by the command.
     */
    const types::PubkeyType &pubkey() const;

    std::string toString() const override;

    bool operator==(const ModelType &rhs) const override;

   private:
    const types::PubkeyType pubkey_;
  };
}  // namespace shared_model

#endif  // IROHA_SHARED_MODEL_REMOVE_PEER_HPP
