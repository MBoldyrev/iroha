/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_PROTO_COMMON_OBJECTS_FACTORY_HPP
#define IROHA_PROTO_COMMON_OBJECTS_FACTORY_HPP

#include <regex>

#include "backend/protobuf/common_objects/account.hpp"
#include "backend/protobuf/common_objects/account_asset.hpp"
#include "backend/protobuf/common_objects/asset.hpp"
#include "backend/protobuf/common_objects/domain.hpp"
#include "backend/protobuf/common_objects/peer.hpp"
#include "backend/protobuf/common_objects/signature.hpp"
#include "common/result.hpp"
#include "interfaces/common_objects/common_objects_factory.hpp"
#include "primitive.pb.h"
#include "validators/answer.hpp"
#include "validators/validators_common.hpp"

namespace shared_model {
  /**
   * ProtoCommonObjectsFactory constructs protobuf-based objects.
   * It performs stateless validation with provided validator
   * @tparam Validator
   */
  template <typename Validator>
  class ProtoCommonObjectsFactory : public CommonObjectsFactory {
   public:
    ProtoCommonObjectsFactory(
        std::shared_ptr<validation::ValidatorsConfig> config)
        : validator_(config) {}

    FactoryResult<std::unique_ptr<Peer>> createPeer(
        const types::AddressType &address,
        const types::PubkeyType &public_key,
        const boost::optional<types::TLSCertificateType> &tls_certificate =
            boost::none) override {
      iroha::protocol::Peer peer;
      peer.set_address(address);
      peer.set_peer_key(public_key.hex());
      if (tls_certificate) {
        peer.set_tls_certificate(*tls_certificate);
      }
      auto proto_peer = std::make_unique<Peer>(std::move(peer));

      auto errors =
          validate(*proto_peer, [this](const auto &peer, auto &reasons) {
            validator_.validatePeer(reasons, peer);
          });

      if (errors) {
        return iroha::expected::makeError(errors.reason());
      }

      return iroha::expected::makeValue<std::unique_ptr<Peer>>(
          std::move(proto_peer));
    }

    FactoryResult<std::unique_ptr<Account>> createAccount(
        const types::AccountIdType &account_id,
        const types::DomainIdType &domain_id,
        types::QuorumType quorum,
        const types::JsonType &jsonData) override {
      iroha::protocol::Account account;
      account.set_account_id(account_id);
      account.set_domain_id(domain_id);
      account.set_quorum(quorum);
      account.set_json_data(jsonData);

      auto proto_account = std::make_unique<Account>(std::move(account));

      auto errors =
          validate(*proto_account, [this](const auto &account, auto &reasons) {
            validator_.validateAccountId(reasons, account.accountId());
            validator_.validateDomainId(reasons, account.domainId());
            validator_.validateQuorum(reasons, account.quorum());
          });

      if (errors) {
        return iroha::expected::makeError(errors.reason());
      }

      return iroha::expected::makeValue<std::unique_ptr<Account>>(
          std::move(proto_account));
    }

    FactoryResult<std::unique_ptr<AccountAsset>> createAccountAsset(
        const types::AccountIdType &account_id,
        const types::AssetIdType &asset_id,
        const Amount &balance) override {
      iroha::protocol::AccountAsset asset;
      asset.set_account_id(account_id);
      asset.set_asset_id(asset_id);
      asset.set_balance(balance.toStringRepr());

      auto proto_asset = std::make_unique<AccountAsset>(std::move(asset));

      auto errors =
          validate(*proto_asset, [this](const auto &asset, auto &reasons) {
            validator_.validateAccountId(reasons, asset.accountId());
            validator_.validateAssetId(reasons, asset.assetId());
          });

      if (errors) {
        return iroha::expected::makeError(errors.reason());
      }

      return iroha::expected::makeValue<std::unique_ptr<AccountAsset>>(
          std::move(proto_asset));
    }

    FactoryResult<std::unique_ptr<Asset>> createAsset(
        const types::AssetIdType &asset_id,
        const types::DomainIdType &domain_id,
        types::PrecisionType precision) override {
      iroha::protocol::Asset asset;
      asset.set_asset_id(asset_id);
      asset.set_domain_id(domain_id);
      asset.set_precision(precision);

      auto proto_asset = std::make_unique<Asset>(std::move(asset));

      auto errors =
          validate(*proto_asset, [this](const auto &asset, auto &reasons) {
            validator_.validateAssetId(reasons, asset.assetId());
            validator_.validateDomainId(reasons, asset.domainId());
          });

      if (errors) {
        return iroha::expected::makeError(errors.reason());
      }

      return iroha::expected::makeValue<std::unique_ptr<Asset>>(
          std::move(proto_asset));
    }

    FactoryResult<std::unique_ptr<Domain>> createDomain(
        const types::DomainIdType &domain_id,
        const types::RoleIdType &default_role) override {
      iroha::protocol::Domain domain;
      domain.set_domain_id(domain_id);
      domain.set_default_role(default_role);

      auto proto_domain = std::make_unique<Domain>(std::move(domain));

      auto errors =
          validate(*proto_domain, [this](const auto &domain, auto &reason) {
            validator_.validateDomainId(reason, domain.domainId());
            validator_.validateRoleId(reason, domain.defaultRole());
          });

      if (errors) {
        return iroha::expected::makeError(errors.reason());
      }

      return iroha::expected::makeValue<std::unique_ptr<Domain>>(
          std::move(proto_domain));
    }

    FactoryResult<std::unique_ptr<Signature>> createSignature(
        const types::PubkeyType &key,
        const Signature::SignedType &signed_data) override {
      iroha::protocol::Signature signature;
      signature.set_public_key(key.hex());
      signature.set_signature(signed_data.hex());

      auto proto_singature = std::make_unique<Signature>(std::move(signature));

      auto errors = validate(
          *proto_singature, [this](const auto &signature, auto &reason) {
            validator_.validatePubkey(reason, signature.publicKey());
          });

      if (errors) {
        return iroha::expected::makeError(errors.reason());
      }

      return iroha::expected::makeValue<std::unique_ptr<Signature>>(
          std::move(proto_singature));
    }

   private:
    /**
     * Perform validation of a given object
     * @param o - object to be validated
     * @param f - function which populates reason parameter with errors.
     * second parameter (reasons) must be passed by non-const reference
     * @return validation result
     */
    template <typename T, typename ValidationFunc>
    validation::Answer validate(const T &o, ValidationFunc &&f) const {
      shared_model::validation::Answer errors;
      validation::ReasonsGroupType reasons;
      f(o, reasons);
      if (not reasons.second.empty()) {
        errors.addReason(std::move(reasons));
      }
      return errors;
    }

    Validator validator_;
  };
}  // namespace shared_model

#endif  // IROHA_PROTO_COMMON_OBJECTS_FACTORY_HPP
