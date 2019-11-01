/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_CREATE_ACCOUNT_HPP
#define IROHA_SHARED_MODEL_CREATE_ACCOUNT_HPP

#include "interfaces/base/model_primitive.hpp"

#include "commands.pb.h"
#include "cryptography/public_key.hpp"

#include "interfaces/common_objects/types.hpp"

namespace shared_model {

  /**
   * Create acccount in Iroha domain
   */
  class CreateAccount : public ModelPrimitive<CreateAccount> {
   public:
    explicit CreateAccount(iroha::protocol::Command &command);

    /**
     * @return Name of the account to create in Iroha
     */
    const types::AccountNameType &accountName() const;
    /**
     * @return Iroha domain in which account will be created
     */
    const types::DomainIdType &domainId() const;
    /**
     * @return Initial account public key
     */
    const types::PubkeyType &pubkey() const;

    std::string toString() const override;

    bool operator==(const ModelType &rhs) const override;

   private:
    const iroha::protocol::CreateAccount &create_account_;

    const types::PubkeyType pubkey_;
  };
}  // namespace shared_model

#endif  // IROHA_SHARED_MODEL_CREATE_ACCOUNT_HPP
