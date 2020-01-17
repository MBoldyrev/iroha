/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_AMETSUCHI_NEWSTORAGE_WSV_TYPES_HPP
#define IROHA_AMETSUCHI_NEWSTORAGE_WSV_TYPES_HPP

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/optional/optional.hpp>
#include <string>
#include "interfaces/permissions.hpp"

namespace iroha {
  namespace newstorage {

    using ID = std::string;
    using AccountID = ID;
    using shared_model::interface::GrantablePermissionSet;
    using shared_model::interface::RolePermissionSet;
    using GrantablePermission = shared_model::interface::permissions::Grantable;
    using RolePermission = shared_model::interface::permissions::Role;
    using RoleID = ID;
    using DomainID = ID;
    using AssetID = ID;
    using PK = std::string;
    using NetworkAddress = std::string;
    using uint256_t = boost::multiprecision::uint256_t;
    using PeerTlsCertificate = std::string;

    struct AccountAssetView {
      const AssetID &id;
      const std::string &balance;
      uint8_t precision;
    };

    struct AssetView {
      const AssetID &id;
      const DomainID &domain;
      uint8_t precision;
    };

    struct DomainView {
      const DomainID &id;
      const RoleID &default_role;
    };

    struct PeerView {
      const NetworkAddress &address;
      const PK &pub_key;
      boost::optional<const PeerTlsCertificate &> tls_certificate;
    };

    struct RoleView {
      const RoleID &id;
      const std::string &permissions;
    };

  }  // namespace newstorage
}  // namespace iroha

#endif
