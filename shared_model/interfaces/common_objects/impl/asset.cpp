/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "interfaces/common_objects/asset.hpp"

#include "utils/string_builder.hpp"

using namespace shared_model;

Asset::Asset(const types::AssetIdType &asset_id,
             const types::DomainIdType &domain_id,
             types::PrecisionType precision)
    : Asset([&]() -> TransportType {
        iroha::protocol::Asset asset;
        asset.set_asset_id(asset_id);
        asset.set_domain_id(domain_id);
        asset.set_precision(precision);
        return asset;
      }()) {}

const types::AssetIdType &Asset::assetId() const override {
  return proto_->asset_id();
}

const types::DomainIdType &Asset::domainId() const override {
  return proto_->domain_id();
}

types::PrecisionType Asset::precision() const override {
  return proto_->precision();
}

std::string Asset::toString() const override {
  return detail::PrettyStringBuilder()
      .init("Asset")
      .append("assetId", assetId())
      .append("domainId", domainId())
      .append("precision", std::to_string(precision()))
      .finalize();
}

bool Asset::operator==(const ModelType &rhs) const override {
  return assetId() == rhs.assetId() and domainId() == rhs.domainId()
      and precision() == rhs.precision();
}
