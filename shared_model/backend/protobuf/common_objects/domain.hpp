/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "interfaces/common_objects/domain.hpp"

#include "utils/string_builder.hpp"

using namespace shared_model;

Domain::Domain(const types::DomainIdType &domain_id,
               const types::RoleIdType &default_role)
    : Domain([&]() -> TransportType {
        iroha::protocol::Domain domain;
        domain.set_domain_id(domain_id);
        domain.set_default_role(default_role);
        return domain;
      }()) {}

const types::DomainIdType &Domain::domainId() const override {
  return proto_->domain_id();
}

const types::RoleIdType &Domain::defaultRole() const override {
  return proto_->default_role();
}

std::string Domain::toString() const override {
  return detail::PrettyStringBuilder()
      .init("Domain")
      .append("domainId", domainId())
      .append("defaultRole", defaultRole())
      .finalize();
}

bool Domain::operator==(const ModelType &rhs) const override {
  return domainId() == rhs.domainId() and defaultRole() == rhs.defaultRole();
}
