/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_PROTO_SET_ACCOUNT_DETAIL_HPP
#define IROHA_PROTO_SET_ACCOUNT_DETAIL_HPP

#include "interfaces/commands/set_account_detail.hpp"

#include "commands.pb.h"

namespace shared_model {
  class SetAccountDetail final : public SetAccountDetail {
   public:
    explicit SetAccountDetail(iroha::protocol::Command &command);

    const types::AccountIdType &accountId() const override;

    const types::AccountDetailKeyType &key() const override;

    const types::AccountDetailValueType &value() const override;

   private:
    const iroha::protocol::SetAccountDetail &set_account_detail_;
  };

}  // namespace shared_model

#endif  // IROHA_PROTO_SET_ACCOUNT_DETAIL_HPP
