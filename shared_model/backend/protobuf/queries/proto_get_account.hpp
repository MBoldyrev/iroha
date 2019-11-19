/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_PROTO_GET_ACCOUNT_H
#define IROHA_PROTO_GET_ACCOUNT_H

#include "interfaces/queries/get_account.hpp"

#include "queries.pb.h"

namespace shared_model {
  class GetAccount final : public GetAccount {
   public:
    explicit GetAccount(iroha::protocol::Query &query);

    const types::AccountIdType &accountId() const override;

   private:
    // ------------------------------| fields |-------------------------------
    const iroha::protocol::GetAccount &account_;
  };

}  // namespace shared_model

#endif  // IROHA_PROTO_GET_ACCOUNT_H
