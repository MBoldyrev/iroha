/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_CREATE_DOMAIN_HPP
#define IROHA_SHARED_MODEL_CREATE_DOMAIN_HPP

#include "interfaces/base/model_primitive.hpp"

#include "commands.pb.h"

#include "interfaces/common_objects/types.hpp"

namespace shared_model {
  /**
   * Create domain in Iroha
   */
  class CreateDomain : public ModelPrimitive<CreateDomain> {
   public:
    explicit CreateDomain(iroha::protocol::Command &command);

    /**
     * @return Id of the domain to create
     */
    const types::DomainIdType &domainId() const;
    /**
     * @return default role of the user in the domain
     */
    const types::RoleIdType &userDefaultRole() const;

    std::string toString() const override;

    bool operator==(const ModelType &rhs) const override;

   private:
    const iroha::protocol::CreateDomain &create_domain_;
  };
}  // namespace shared_model

#endif  // IROHA_SHARED_MODEL_CREATE_DOMAIN_HPP
