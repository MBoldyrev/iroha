/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_REMOVE_SIGNATORY_HPP
#define IROHA_SHARED_MODEL_REMOVE_SIGNATORY_HPP

#include "interfaces/base/model_primitive.hpp"

#include "interfaces/common_objects/types.hpp"

namespace shared_model {
  /**
   * Remove signatory from the account
   */
  class RemoveSignatory : public ModelPrimitive<RemoveSignatory> {
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
  };
}  // namespace shared_model

#endif  // IROHA_SHARED_MODEL_REMOVE_SIGNATORY_HPP
