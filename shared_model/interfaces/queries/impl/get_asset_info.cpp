/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "interfaces/queries/get_asset_info.hpp"

std::string GetAssetInfo::toString() const {
  return detail::PrettyStringBuilder()
      .init("GetAssetInfo")
      .append("asset_id", assetId())
      .finalize();
}

bool GetAssetInfo::operator==(const ModelType &rhs) const {
  return assetId() == rhs.assetId();
}
