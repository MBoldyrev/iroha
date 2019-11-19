/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_SET_QUORUM_HPP
#define IROHA_SHARED_MODEL_SET_QUORUM_HPP

#include "interfaces/base/model_primitive.hpp"

#include "commands.pb.h"

#include "interfaces/common_objects/types.hpp"

namespace shared_model {
  /**
   * Set quorum of the account
   */
  class SetQuorum : public ModelPrimitive<SetQuorum> {
   public:
    explicit SetQuorum(iroha::protocol::Command &command);

    /**
     * @return Id of the account to set quorum
     */
    const types::AccountIdType &accountId() const;
    /**
     * @return value of a new quorum
     */
    types::QuorumType newQuorum() const;

    std::string toString() const override;

    bool operator==(const ModelType &rhs) const override;

   private:
    const iroha::protocol::SetAccountQuorum &set_account_quorum_;
  };
}  // namespace shared_model

#endif  // IROHA_SHARED_MODEL_SET_QUORUM_HPP
