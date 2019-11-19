/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_INTERFACE_MODEL_QUERY_ACCOUNT_DETAIL_RECORD_ID_HPP
#define IROHA_SHARED_INTERFACE_MODEL_QUERY_ACCOUNT_DETAIL_RECORD_ID_HPP

#include <boost/optional.hpp>

#include "interfaces/base/model_primitive.hpp"
#include "interfaces/common_objects/types.hpp"
#include "primitive.pb.h"

namespace shared_model {

  /// Provides query metadata for account detail list pagination.
  class AccountDetailRecordId : public ModelPrimitive<AccountDetailRecordId> {
   public:
    using TransportType = iroha::protocol::AccountDetailRecordId;

    explicit AccountDetailRecordId(TransportType &proto);

    explicit AccountDetailRecordId(const AccountDetailRecordId &o);

    /// Get the writer.
    types::AccountIdType writer() const;

    /// Get the key.
    types::AccountDetailKeyType key() const;

    std::string toString() const override;

    bool operator==(const ModelType &rhs) const override;

   private:
    TransportType &proto_;
  };

}  // namespace shared_model

#endif  // IROHA_SHARED_INTERFACE_MODEL_QUERY_ACCOUNT_DETAIL_RECORD_ID_HPP
