/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_REMOVE_SIGNATORY_HPP
#define IROHA_SHARED_MODEL_REMOVE_SIGNATORY_HPP

#include "interfaces/base/model_primitive.hpp"

#include "commands.pb.h"
#include "cryptography/public_key.hpp"

#include "interfaces/common_objects/types.hpp"

namespace shared_model {
  /**
   * Remove signatory from the account
   */
  class RemoveSignatory : public ModelPrimitive<RemoveSignatory> {
    explicit RemoveSignatory(iroha::protocol::Command &command);

   public:
    /**
     * @return account from which remove signatory
     */
    const types::AccountIdType &accountId() const;
    /**
     * @return Public key to remove from account
     */
    const types::PubkeyType &pubkey() const;

    std::string toString() const override;

    bool operator==(const ModelType &rhs) const override;

   private:
    const iroha::protocol::RemoveSignatory &remove_signatory_;

    const types::PubkeyType pubkey_;
  };
}  // namespace shared_model

#endif  // IROHA_SHARED_MODEL_REMOVE_SIGNATORY_HPP
