/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_PROTO_COMPARE_AND_SET_ACCOUNT_DETAIL_HPP
#define IROHA_PROTO_COMPARE_AND_SET_ACCOUNT_DETAIL_HPP

#include "interfaces/commands/compare_and_set_account_detail.hpp"

#include "commands.pb.h"

namespace shared_model {
  class CompareAndSetAccountDetail final : public CompareAndSetAccountDetail {
   public:
    explicit CompareAndSetAccountDetail(iroha::protocol::Command &command);

    const types::AccountIdType &accountId() const override;

    const types::AccountDetailKeyType &key() const override;

    const types::AccountDetailValueType &value() const override;

    const boost::optional<types::AccountDetailValueType> oldValue()
        const override;

   private:
    const iroha::protocol::CompareAndSetAccountDetail
        &compare_and_set_account_detail_;
  };

}  // namespace shared_model

#endif  // IROHA_PROTO_COMPARE_AND_SET_ACCOUNT_DETAIL_HPP
