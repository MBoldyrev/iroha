/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_PLAIN_ACCOUNT_HPP
#define IROHA_SHARED_MODEL_PLAIN_ACCOUNT_HPP

#include "interfaces/common_objects/account.hpp"

#include "interfaces/common_objects/types.hpp"

namespace shared_model {
  namespace plain {

    class Account final : public Account {
     public:
      Account(const types::AccountIdType &account_id,
              const types::DomainIdType &domain_id,
              types::QuorumType quorum,
              const types::JsonType &json_data);

      const types::AccountIdType &accountId() const override;

      const types::DomainIdType &domainId() const override;

      types::QuorumType quorum() const override;

      const types::JsonType &jsonData() const override;

     private:
      const types::AccountIdType account_id_;
      const types::DomainIdType domain_id_;
      types::QuorumType quorum_;
      const types::JsonType json_data_;
    };
  }  // namespace plain
}  // namespace shared_model

#endif  // IROHA_SHARED_MODEL_PLAIN_ACCOUNT_HPP
