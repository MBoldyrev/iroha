/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "interfaces/common_objects/account_asset.hpp"

#include "utils/string_builder.hpp"

AccountAsset::AccountAsset(const types::AccountIdType &account_id,
                           const types::AssetIdType &asset_id,
                           const Amount &balance)
    : AccountAsset([&]() -> TransportType {
        iroha::protocol::AccountAsset asset;
        asset.set_account_id(account_id);
        asset.set_asset_id(asset_id);
        asset.set_balance(balance.toStringRepr());
        return asset;
      }()) {}

const types::AccountIdType &AccountAsset::accountId() const override {
  return proto_->account_id();
}

const types::AssetIdType &AccountAsset::assetId() const override {
  return proto_->asset_id();
}

const Amount &AccountAsset::balance() const override {
  return balance_;
}

std::string AccountAsset::toString() const override {
  return detail::PrettyStringBuilder()
      .init("AccountAsset")
      .append("accountId", accountId())
      .append("assetId", assetId())
      .append("balance", balance().toString())
      .finalize();
}

bool AccountAsset::operator==(const ModelType &rhs) const override {
  return accountId() == rhs.accountId() and assetId() == rhs.assetId()
      and balance() == rhs.balance();
}
