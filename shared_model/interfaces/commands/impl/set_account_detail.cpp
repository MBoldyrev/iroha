/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "interfaces/commands/set_account_detail.hpp"

std::string SetAccountDetail::toString() const {
  return detail::PrettyStringBuilder()
      .init("SetAccountDetail")
      .append("account_id", accountId())
      .append("key", key())
      .append("value", value())
      .finalize();
}

bool SetAccountDetail::operator==(const ModelType &rhs) const {
  return accountId() == rhs.accountId() and key() == rhs.key()
      and value() == rhs.value();
}
