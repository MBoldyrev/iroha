/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "backend/protobuf/queries/proto_get_account_detail.hpp"

GetAccountDetail::GetAccountDetail(iroha::protocol::Query &query)
    : query_{query},
      account_detail_{query.payload().get_account_detail()},
      pagination_meta_{[&]() -> decltype(pagination_meta_) {
        if (query.payload().get_account_detail().has_pagination_meta()) {
          return AccountDetailPaginationMeta{*query.mutable_payload()
                                                  ->mutable_get_account_detail()
                                                  ->mutable_pagination_meta()};
        }
        return boost::none;
      }()} {}

const interface::types::AccountIdType &GetAccountDetail::accountId() const {
  return account_detail_.opt_account_id_case()
      ? account_detail_.account_id()
      : query_.payload().meta().creator_account_id();
}

boost::optional<interface::types::AccountDetailKeyType> GetAccountDetail::key()
    const {
  return account_detail_.opt_key_case()
      ? boost::make_optional(account_detail_.key())
      : boost::none;
}

boost::optional<interface::types::AccountIdType> GetAccountDetail::writer()
    const {
  return account_detail_.opt_writer_case()
      ? boost::make_optional(account_detail_.writer())
      : boost::none;
}

boost::optional<const interface::AccountDetailPaginationMeta &>
GetAccountDetail::paginationMeta() const {
  if (pagination_meta_) {
    return *pagination_meta_;
  }
  return boost::none;
}
