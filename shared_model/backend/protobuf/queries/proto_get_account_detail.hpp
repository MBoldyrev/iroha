/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_PROTO_GET_ACCOUNT_DETAIL_HPP
#define IROHA_PROTO_GET_ACCOUNT_DETAIL_HPP

#include "interfaces/queries/get_account_detail.hpp"

#include <boost/optional.hpp>
#include "backend/protobuf/queries/proto_account_detail_pagination_meta.hpp"
#include "queries.pb.h"

namespace shared_model {
  class GetAccountDetail final : public GetAccountDetail {
   public:
    explicit GetAccountDetail(iroha::protocol::Query &query);

    const types::AccountIdType &accountId() const override;

    boost::optional<types::AccountDetailKeyType> key() const override;

    boost::optional<types::AccountIdType> writer() const override;

    boost::optional<const AccountDetailPaginationMeta &> paginationMeta()
        const override;

   private:
    // ------------------------------| fields |-------------------------------

    const iroha::protocol::Query &query_;
    const iroha::protocol::GetAccountDetail &account_detail_;
    const boost::optional<const AccountDetailPaginationMeta> pagination_meta_;
  };
}  // namespace shared_model

#endif  // IROHA_PROTO_GET_ACCOUNT_DETAIL_HPP
