/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_ACCOUNT_RESPONSE_HPP
#define IROHA_SHARED_MODEL_ACCOUNT_RESPONSE_HPP

#include "interfaces/base/model_primitive.hpp"

#include "backend/protobuf/common_objects/account.hpp"
#include "qry_responses.pb.h"

#include "interfaces/common_objects/account.hpp"
#include "interfaces/common_objects/types.hpp"

namespace shared_model {
  /**
   * Provide response with account
   */
  class AccountResponse : public ModelPrimitive<AccountResponse> {
   public:
    explicit AccountResponse(iroha::protocol::QueryResponse &query_response);

    /// Collection of role_id types
    using AccountRolesIdType = std::vector<types::RoleIdType>;

    /**
     * @return the fetched account.
     */
    const Account &account() const;

    /**
     * @return roles attached to the account
     */
    const AccountRolesIdType &roles() const;

    std::string toString() const override;

    bool operator==(const ModelType &rhs) const override;

   private:
    const iroha::protocol::AccountResponse &account_response_;

    const AccountRolesIdType account_roles_;

    Account account_;
  };
}  // namespace shared_model
#endif  // IROHA_SHARED_MODEL_ACCOUNT_RESPONSE_HPP
