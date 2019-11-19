/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_PLAIN_DOMAIN_HPP
#define IROHA_SHARED_MODEL_PLAIN_DOMAIN_HPP

#include "interfaces/common_objects/domain.hpp"

#include "interfaces/common_objects/types.hpp"

namespace shared_model {
  namespace plain {

    class Domain final : public Domain {
     public:
      Domain(const types::DomainIdType &domain_id,
             const types::RoleIdType &default_role_id);

      const types::DomainIdType &domainId() const override;

      const types::RoleIdType &defaultRole() const override;

     private:
      const types::DomainIdType domain_id_;
      const types::RoleIdType default_role_id_;
    };
  }  // namespace plain
}  // namespace shared_model

#endif  // IROHA_SHARED_MODEL_PLAIN_DOMAIN_HPP
