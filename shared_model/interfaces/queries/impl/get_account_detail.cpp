/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "interfaces/queries/get_account_detail.hpp"

using namespace shared_model;

std::string GetAccountDetail::toString() const {
  return detail::PrettyStringBuilder()
      .init("GetAccountDetail")
      .append("account_id", accountId())
      .append("key", key() ? *key() : "")
      .append("writer", writer() ? *writer() : "")
      .append("pagination_meta",
              paginationMeta() ? paginationMeta()->toString() : "(not set)")
      .finalize();
}

bool GetAccountDetail::operator==(const ModelType &rhs) const {
  return accountId() == rhs.accountId() and key() == rhs.key()
      and writer() == rhs.writer() and paginationMeta() == rhs.paginationMeta();
}
