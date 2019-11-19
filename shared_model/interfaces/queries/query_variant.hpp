/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_QUERY_VARIANT_HPP
#define IROHA_SHARED_MODEL_QUERY_VARIANT_HPP

#include "interfaces/queries/query.hpp"

#include <boost/variant.hpp>

namespace boost {
  extern template class variant<
      const shared_model::GetAccount &,
      const shared_model::GetSignatories &,
      const shared_model::GetAccountTransactions &,
      const shared_model::GetAccountAssetTransactions &,
      const shared_model::GetTransactions &,
      const shared_model::GetAccountAssets &,
      const shared_model::GetAccountDetail &,
      const shared_model::GetRoles &,
      const shared_model::GetRolePermissions &,
      const shared_model::GetAssetInfo &,
      const shared_model::GetPendingTransactions &,
      const shared_model::GetPeers &>;
}  // namespace boost

#endif  // IROHA_SHARED_MODEL_QUERY_VARIANT_HPP
