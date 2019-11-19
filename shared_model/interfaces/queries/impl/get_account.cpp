/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "interfaces/queries/get_account.hpp"

namespace shared_model {

  std::string GetAccount::toString() const {
    return detail::PrettyStringBuilder()
        .init("GetAccount")
        .append("account_id", accountId())
        .finalize();
  }

  bool GetAccount::operator==(const ModelType &rhs) const {
    return accountId() == rhs.accountId();
  }

}  // namespace shared_model
