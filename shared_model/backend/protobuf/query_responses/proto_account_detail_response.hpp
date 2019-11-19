/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_PROTO_ACCOUNT_DETAIL_RESPONSE_HPP
#define IROHA_PROTO_ACCOUNT_DETAIL_RESPONSE_HPP

#include "interfaces/query_responses/account_detail_response.hpp"

#include "backend/protobuf/common_objects/account_asset.hpp"
#include "backend/protobuf/queries/proto_account_detail_record_id.hpp"
#include "qry_responses.pb.h"

namespace shared_model {
  class AccountDetailResponse final : public AccountDetailResponse {
   public:
    explicit AccountDetailResponse(
        iroha::protocol::QueryResponse &query_response);

    const types::DetailType &detail() const override;

    size_t totalNumber() const override;

    boost::optional<const shared_model::AccountDetailRecordId &> nextRecordId()
        const override;

   private:
    const iroha::protocol::AccountDetailResponse &account_detail_response_;
    const boost::optional<const AccountDetailRecordId> next_record_id_;
  };
}  // namespace shared_model

#endif  // IROHA_PROTO_ACCOUNT_DETAIL_RESPONSE_HPP
