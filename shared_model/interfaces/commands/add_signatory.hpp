/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_ADD_SIGNATORY_HPP
#define IROHA_SHARED_MODEL_ADD_SIGNATORY_HPP

#include "interfaces/base/model_primitive.hpp"

#include "interfaces/common_objects/types.hpp"

namespace shared_model {

  /**
   * Add new signatory to account
   */
  class AddSignatory : public ModelPrimitive<AddSignatory> {
   public:
    /**
     * @return New signatory is identified with public key
     */
    const types::PubkeyType &pubkey() const;
    /**
     * @return Account to which add new signatory
     */
    const types::AccountIdType &accountId() const;

    std::string toString() const override;

    bool operator==(const ModelType &rhs) const override;
  };
}  // namespace shared_model

#endif  // IROHA_SHARED_MODEL_ADD_SIGNATORY_HPP
