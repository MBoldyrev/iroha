/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_DOMAIN_HPP
#define IROHA_SHARED_MODEL_DOMAIN_HPP

#include "interfaces/common_objects/trivial_proto.hpp"
#include "interfaces/common_objects/types.hpp"
#include "qry_responses.pb.h"

namespace shared_model {

  /**
   * Domain object represents administrative unit within the system
   */
  class Domain : public TrivialProto<Domain, iroha::protocol::Domain> {
   public:
    template <typename T>
    explicit Domain(T &&domain) : TrivialProto(std::forward<T>(domain)) {}

    Domain(const types::DomainIdType &domain_id,
           const types::RoleIdType &default_role);

    /**
     * @return Identity of domain
     */
    const types::DomainIdType &domainId() const;

    /**
     * @return Default role of domain
     */
    const types::RoleIdType &defaultRole() const;

    /**
     * Stringify the data.
     * @return the content of asset.
     */
    std::string toString() const override;

    /**
     * Checks equality of objects inside
     * @param rhs - other wrapped value
     * @return true, if wrapped objects are same
     */
    bool operator==(const ModelType &rhs) const override;
  };
}  // namespace shared_model

#endif  // IROHA_SHARED_MODEL_DOMAIN_HPP
