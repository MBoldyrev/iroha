/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_PROTO_REMOVE_SIGNATORY_HPP
#define IROHA_PROTO_REMOVE_SIGNATORY_HPP

#include "interfaces/commands/remove_signatory.hpp"

#include "commands.pb.h"
#include "cryptography/public_key.hpp"

namespace shared_model {

  class RemoveSignatory final : public RemoveSignatory {
   public:
    explicit RemoveSignatory(iroha::protocol::Command &command);

    const types::AccountIdType &accountId() const override;

    const types::PubkeyType &pubkey() const override;

   private:
    const iroha::protocol::RemoveSignatory &remove_signatory_;

    const types::PubkeyType pubkey_;
  };

}  // namespace shared_model

#endif  // IROHA_PROTO_REMOVE_SIGNATORY_HPP
