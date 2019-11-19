/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_GET_SIGNATORIES_HPP
#define IROHA_SHARED_MODEL_GET_SIGNATORIES_HPP

#include "interfaces/base/model_primitive.hpp"

#include "interfaces/common_objects/types.hpp"
#include "queries.pb.h"

namespace shared_model {

  /**
   * Query for getting all signatories attached to account
   */
  class GetSignatories : public ModelPrimitive<GetSignatories> {
   public:
    explicit GetSignatories(iroha::protocol::Query &query);

    /**
     * @return account_id of requested signatories
     */
    const types::AccountIdType &accountId() const;

    std::string toString() const override;

    bool operator==(const ModelType &rhs) const override;

   private:
    // ------------------------------| fields |-------------------------------

    const iroha::protocol::GetSignatories &account_signatories_;
  };
}  // namespace shared_model

#endif  // IROHA_SHARED_MODEL_GET_SIGNATORIES_HPP
