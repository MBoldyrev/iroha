/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_INTERFACE_MODEL_QUERY_TX_PAGINATION_META_HPP
#define IROHA_SHARED_INTERFACE_MODEL_QUERY_TX_PAGINATION_META_HPP

#include <boost/optional.hpp>
#include "interfaces/base/model_primitive.hpp"
#include "interfaces/common_objects/types.hpp"

namespace shared_model {

  /// Provides query metadata for any transaction list pagination.
  class TxPaginationMeta : public ModelPrimitive<TxPaginationMeta> {
   public:
    /// Get the requested page size.
    types::TransactionsNumberType pageSize() const;

    /// Get the first requested transaction hash, if provided.
    boost::optional<types::HashType> firstTxHash() const;

    std::string toString() const override;

    bool operator==(const ModelType &rhs) const override;
  };

}  // namespace shared_model

#endif  // IROHA_SHARED_INTERFACE_MODEL_QUERY_TX_PAGINATION_META_HPP
