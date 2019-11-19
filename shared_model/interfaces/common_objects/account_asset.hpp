/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_ACCOUNT_ASSET_HPP
#define IROHA_SHARED_MODEL_ACCOUNT_ASSET_HPP

#include "interfaces/base/model_primitive.hpp"

#include "interfaces/common_objects/amount.hpp"
#include "interfaces/common_objects/trivial_proto.hpp"
#include "interfaces/common_objects/types.hpp"
#include "interfaces/util.hpp"
#include "qry_responses.pb.h"
#include "utils/string_builder.hpp"

namespace shared_model {

  /**
   * Representation of wallet in system
   */
  class AccountAsset : public ModelPrimitive<AccountAsset> {
   public:
    template <typename AccountAssetType>
    explicit AccountAsset(AccountAssetType &&accountAssetType)
        : TrivialProto(std::forward<AccountAssetType>(accountAssetType)) {}

    AccountAsset(const AccountAsset &o) : AccountAsset(o.proto_) {}

    AccountAsset(AccountAsset &&o) noexcept
        : AccountAsset(std::move(o.proto_)) {}

    /**
     * @return Identity of user, for fetching data
     */
    const types::AccountIdType &accountId() const;

    /**
     * @return Identity of asset, for fetching data
     */
    const types::AssetIdType &assetId() const;

    /**
     * @return Current balance
     */
    const Amount &balance() const;

    /**
     * Stringify the data.
     * @return the content of account asset.
     */
    std::string toString() const override {
      return detail::PrettyStringBuilder()
          .init("AccountAsset")
          .append("accountId", accountId())
          .append("assetId", assetId())
          .append("balance", balance().toString())
          .finalize();
    }

    /**
     * Checks equality of objects inside
     * @param rhs - other wrapped value
     * @return true, if wrapped objects are same
     */
    bool operator==(const ModelType &rhs) const override {
      return accountId() == rhs.accountId() and assetId() == rhs.assetId()
          and balance() == rhs.balance();
    }

   private:
    const Amount balance_{proto_->balance()};
  };
}  // namespace shared_model
#endif  // IROHA_SHARED_MODEL_ACCOUNT_ASSET_HPP
