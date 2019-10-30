/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_PROTO_ACCOUNT_ASSET_HPP
#define IROHA_PROTO_ACCOUNT_ASSET_HPP

#include "interfaces/common_objects/account_asset.hpp"

#include "backend/protobuf/common_objects/trivial_proto.hpp"
#include "backend/protobuf/util.hpp"
#include "qry_responses.pb.h"

namespace shared_model {
  class AccountAsset final
      : public TrivialProto<AccountAsset, iroha::protocol::AccountAsset> {
   public:
    template <typename AccountAssetType>
    explicit AccountAsset(AccountAssetType &&accountAssetType)
        : TrivialProto(std::forward<AccountAssetType>(accountAssetType)) {}

    AccountAsset(const AccountAsset &o) : AccountAsset(o.proto_) {}

    AccountAsset(AccountAsset &&o) noexcept
        : AccountAsset(std::move(o.proto_)) {}

    const types::AccountIdType &accountId() const override {
      return proto_->account_id();
    }

    const types::AssetIdType &assetId() const override {
      return proto_->asset_id();
    }

    const Amount &balance() const override {
      return balance_;
    }

   private:
    const Amount balance_{proto_->balance()};
  };
}  // namespace shared_model

#endif  // IROHA_PROTO_ACCOUNT_ASSET_HPP
