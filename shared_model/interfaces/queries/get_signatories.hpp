/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_GET_SIGNATORIES_HPP
#define IROHA_SHARED_MODEL_GET_SIGNATORIES_HPP

#include "interfaces/base/model_primitive.hpp"
#include "interfaces/common_objects/types.hpp"

namespace shared_model {

  /**
   * Query for getting all signatories attached to account
   */
  class GetSignatories : public ModelPrimitive<GetSignatories> {
   public:
    /**
     * @return account_id of requested signatories
     */
    const types::AccountIdType &accountId() const;

    std::string toString() const override;

    bool operator==(const ModelType &rhs) const override;
  };
}  // namespace shared_model

#endif  // IROHA_SHARED_MODEL_GET_SIGNATORIES_HPP
