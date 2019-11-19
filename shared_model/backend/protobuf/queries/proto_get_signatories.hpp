/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_PROTO_GET_SIGNATORIES_H
#define IROHA_PROTO_GET_SIGNATORIES_H

#include "interfaces/queries/get_signatories.hpp"

#include "queries.pb.h"

namespace shared_model {
  class GetSignatories final : public GetSignatories {
   public:
    explicit GetSignatories(iroha::protocol::Query &query);

    const types::AccountIdType &accountId() const override;

   private:
    // ------------------------------| fields |-------------------------------

    const iroha::protocol::GetSignatories &account_signatories_;
  };

}  // namespace shared_model

#endif  // IROHA_PROTO_GET_SIGNATORIES_H
