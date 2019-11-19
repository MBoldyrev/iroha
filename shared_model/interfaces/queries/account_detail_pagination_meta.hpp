/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_INTERFACE_MODEL_QUERY_ACCOUNT_DETAIL_PAGINATION_META_HPP
#define IROHA_SHARED_INTERFACE_MODEL_QUERY_ACCOUNT_DETAIL_PAGINATION_META_HPP

#include <boost/optional.hpp>
#include "interfaces/base/model_primitive.hpp"
#include "interfaces/common_objects/types.hpp"
#include "interfaces/queries/account_detail_record_id.hpp"

namespace shared_model {

  /// Provides query metadata for account detail list pagination.
  class AccountDetailPaginationMeta
      : public ModelPrimitive<AccountDetailPaginationMeta> {
   public:
    /// Get the requested page size.
    size_t pageSize() const;

    /// Get the first requested record id, if provided.
    boost::optional<const AccountDetailRecordId &> firstRecordId() const;

    std::string toString() const override;

    bool operator==(const ModelType &rhs) const override;
  };

}  // namespace shared_model

#endif  // IROHA_SHARED_INTERFACE_MODEL_QUERY_ACCOUNT_DETAIL_PAGINATION_META_HPP
