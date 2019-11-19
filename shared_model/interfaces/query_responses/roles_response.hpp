/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_ROLES_RESPONSE_HPP
#define IROHA_SHARED_MODEL_ROLES_RESPONSE_HPP

#include "interfaces/base/model_primitive.hpp"

#include "qry_responses.pb.h"

#include "interfaces/common_objects/types.hpp"

namespace shared_model {
  /**
   * Provide response with all roles of the current system
   */
  class RolesResponse : public ModelPrimitive<RolesResponse> {
   public:
    explicit RolesResponse(iroha::protocol::QueryResponse &query_response);

    /// type of roles collection
    using RolesIdType = std::vector<types::RoleIdType>;

    /**
     * @return all roles of the current system
     */
    const RolesIdType &roles() const;

    std::string toString() const override;

    bool operator==(const ModelType &rhs) const override;

   private:
    const iroha::protocol::RolesResponse &roles_response_;

    const RolesIdType roles_;
  };
}  // namespace shared_model
#endif  // IROHA_SHARED_MODEL_ROLES_RESPONSE_HPP
