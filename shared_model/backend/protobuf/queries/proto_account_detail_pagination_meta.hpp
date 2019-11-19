/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_PROTO_MODEL_QUERY_ACCOUNT_DETAIL_PAGINATION_META_HPP
#define IROHA_SHARED_PROTO_MODEL_QUERY_ACCOUNT_DETAIL_PAGINATION_META_HPP

#include "interfaces/queries/account_detail_pagination_meta.hpp"

#include <boost/optional.hpp>
#include "backend/protobuf/queries/proto_account_detail_record_id.hpp"
#include "interfaces/common_objects/types.hpp"
#include "interfaces/queries/account_detail_record_id.hpp"
#include "queries.pb.h"

namespace shared_model {

  /// Provides query metadata for AccountDetail list pagination.
  class AccountDetailPaginationMeta final : public AccountDetailPaginationMeta {
   public:
    using TransportType = iroha::protocol::AccountDetailPaginationMeta;

    explicit AccountDetailPaginationMeta(TransportType &proto);

    AccountDetailPaginationMeta(const AccountDetailPaginationMeta &o);

    size_t pageSize() const override;

    boost::optional<const AccountDetailRecordId &> firstRecordId()
        const override;

   private:
    TransportType &proto_;
    const boost::optional<const AccountDetailRecordId> first_record_id_;
  };
}  // namespace shared_model

#endif  // IROHA_SHARED_PROTO_MODEL_QUERY_ACCOUNT_DETAIL_PAGINATION_META_HPP
