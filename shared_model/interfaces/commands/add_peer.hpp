/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_ADD_PEER_HPP
#define IROHA_SHARED_MODEL_ADD_PEER_HPP

#include "interfaces/base/model_primitive.hpp"

#include "interfaces/common_objects/peer.hpp"
#include "interfaces/common_objects/types.hpp"

namespace shared_model {

  /**
   * Add new peer to Iroha
   */
  class AddPeer : public ModelPrimitive<AddPeer> {
   public:
    /**
     * Return peer to be added by the command.
     * @return Peer
     */
    const Peer &peer() const;

    std::string toString() const override;

    bool operator==(const ModelType &rhs) const override;
  };
}  // namespace shared_model

#endif  // IROHA_SHARED_MODEL_ADD_PEER_HPP
