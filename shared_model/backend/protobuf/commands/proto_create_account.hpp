/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_PROTO_CREATE_ACCOUNT_HPP
#define IROHA_PROTO_CREATE_ACCOUNT_HPP

#include "interfaces/commands/create_account.hpp"

#include "commands.pb.h"
#include "cryptography/public_key.hpp"

namespace shared_model {

  class CreateAccount final : public CreateAccount {
   public:
    explicit CreateAccount(iroha::protocol::Command &command);

    const types::PubkeyType &pubkey() const override;

    const types::AccountNameType &accountName() const override;

    const types::DomainIdType &domainId() const override;

   private:
    const iroha::protocol::CreateAccount &create_account_;

    const types::PubkeyType pubkey_;
  };

}  // namespace shared_model

#endif  // IROHA_PROTO_CREATE_ACCOUNT_HPP
