/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "interfaces/queries/get_signatories.hpp"

using namespace shared_model;

std::string GetSignatories::toString() const {
  return detail::PrettyStringBuilder()
      .init("GetSignatories")
      .append("account_id", accountId())
      .finalize();
}

bool GetSignatories::operator==(const ModelType &rhs) const {
  return accountId() == rhs.accountId();
}
