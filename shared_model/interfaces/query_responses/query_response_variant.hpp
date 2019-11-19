/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_QUERY_RESPONSE_VARIANT_HPP
#define IROHA_SHARED_MODEL_QUERY_RESPONSE_VARIANT_HPP

#include "interfaces/query_responses/query_response.hpp"

#include <boost/variant.hpp>

namespace boost {
  extern template class variant<
      const shared_model::AccountAssetResponse &,
      const shared_model::AccountDetailResponse &,
      const shared_model::AccountResponse &,
      const shared_model::ErrorQueryResponse &,
      const shared_model::SignatoriesResponse &,
      const shared_model::TransactionsResponse &,
      const shared_model::AssetResponse &,
      const shared_model::RolesResponse &,
      const shared_model::RolePermissionsResponse &,
      const shared_model::TransactionsPageResponse &,
      const shared_model::PendingTransactionsPageResponse &,
      const shared_model::PeersResponse &>;
}

#endif  // IROHA_SHARED_MODEL_QUERY_RESPONSE_VARIANT_HPP
