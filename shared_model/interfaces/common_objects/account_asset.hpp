/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_ACCOUNT_ASSET_HPP
#define IROHA_SHARED_MODEL_ACCOUNT_ASSET_HPP

#include "interfaces/common_objects/amount.hpp"
#include "interfaces/common_objects/trivial_proto.hpp"
#include "interfaces/common_objects/types.hpp"
#include "qry_responses.pb.h"

namespace shared_model {

  /**
   * Representation of wallet in system
   */
  class AccountAsset
      : public TrivialProto<AccountAsset, iroha::protocol::AccountAsset> {
   public:
    template <typename T>
    explicit AccountAsset(T &&accountAssetType)
        : TrivialProto(std::forward<T>(accountAssetType)) {}

    AccountAsset(const types::AccountIdType &account_id,
                 const types::AssetIdType &asset_id,
                 const Amount &balance);

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
    std::string toString() const override;

    /**
     * Checks equality of objects inside
     * @param rhs - other wrapped value
     * @return true, if wrapped objects are same
     */
    bool operator==(const ModelType &rhs) const override;

   private:
    const Amount balance_{proto_->balance()};
  };
}  // namespace shared_model
#endif  // IROHA_SHARED_MODEL_ACCOUNT_ASSET_HPP
