/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_PROTO_ASSET_HPP
#define IROHA_SHARED_MODEL_PROTO_ASSET_HPP

#include "backend/protobuf/common_objects/trivial_proto.hpp"
#include "interfaces/common_objects/asset.hpp"
#include "qry_responses.pb.h"

namespace shared_model {
  class Asset final : public TrivialProto<Asset, iroha::protocol::Asset> {
   public:
    template <typename AssetType>
    explicit Asset(AssetType &&account)
        : TrivialProto(std::forward<AssetType>(account)) {}

    Asset(const Asset &o) : Asset(o.proto_) {}

    Asset(Asset &&o) noexcept : Asset(std::move(o.proto_)) {}

    const types::AssetIdType &assetId() const override {
      return proto_->asset_id();
    }

    const types::DomainIdType &domainId() const override {
      return proto_->domain_id();
    }

    types::PrecisionType precision() const override {
      return proto_->precision();
    }
  };
}  // namespace shared_model

#endif  // IROHA_SHARED_MODEL_PROTO_ASSET_HPP
