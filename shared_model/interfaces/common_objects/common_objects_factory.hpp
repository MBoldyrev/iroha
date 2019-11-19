/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_COMMON_OBJECTS_FACTORY_HPP
#define IROHA_COMMON_OBJECTS_FACTORY_HPP

#include <memory>

#include "common/result.hpp"
#include "interfaces/common_objects/account.hpp"
#include "interfaces/common_objects/account_asset.hpp"
#include "interfaces/common_objects/asset.hpp"
#include "interfaces/common_objects/domain.hpp"
#include "interfaces/common_objects/peer.hpp"
#include "interfaces/common_objects/signature.hpp"
#include "interfaces/common_objects/types.hpp"

namespace shared_model {
  /**
   * CommonObjectsFactory provides methods to construct common objects
   * such as peer, account etc.
   */
  class CommonObjectsFactory {
   public:
    template <typename T>
    using FactoryResult = iroha::expected::Result<T, std::string>;

    /**
     * Create peer instance
     */
    FactoryResult<std::unique_ptr<Peer>> createPeer(
        const types::AddressType &address,
        const types::PubkeyType &public_key,
        const boost::optional<types::TLSCertificateType> &tls_certificate =
            boost::none);

    /**
     * Create account instance
     */
    FactoryResult<std::unique_ptr<Account>> createAccount(
        const types::AccountIdType &account_id,
        const types::DomainIdType &domain_id,
        types::QuorumType quorum,
        const types::JsonType &jsonData);

    /**
     * Create account asset instance
     */
    FactoryResult<std::unique_ptr<AccountAsset>> createAccountAsset(
        const types::AccountIdType &account_id,
        const types::AssetIdType &asset_id,
        const Amount &balance);

    /**
     * Create asset instance
     */
    FactoryResult<std::unique_ptr<Asset>> createAsset(
        const types::AssetIdType &asset_id,
        const types::DomainIdType &domain_id,
        types::PrecisionType precision);

    /**
     * Create domain instance
     */
    FactoryResult<std::unique_ptr<Domain>> createDomain(
        const types::DomainIdType &domain_id,
        const types::RoleIdType &default_role);

    /**
     * Create signature instance
     */
    FactoryResult<std::unique_ptr<Signature>> createSignature(
        const types::PubkeyType &key, const Signature::SignedType &signed_data);

    ~CommonObjectsFactory() = default;
  };
}  // namespace shared_model

#endif  // IROHA_COMMONOBJECTSFACTORY_HPP
