/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_PROTO_SET_QUORUM_HPP
#define IROHA_PROTO_SET_QUORUM_HPP

#include "interfaces/commands/set_quorum.hpp"

#include "commands.pb.h"

namespace shared_model {
  class SetQuorum final : public SetQuorum {
   public:
    explicit SetQuorum(iroha::protocol::Command &command);

    const types::AccountIdType &accountId() const override;

    types::QuorumType newQuorum() const override;

   private:
    const iroha::protocol::SetAccountQuorum &set_account_quorum_;
  };

}  // namespace shared_model

#endif  // IROHA_PROTO_SET_QUORUM_HPP
