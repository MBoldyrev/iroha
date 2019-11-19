/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_PROTO_ACCOUNT_HPP
#define IROHA_SHARED_MODEL_PROTO_ACCOUNT_HPP

#include "backend/protobuf/common_objects/trivial_proto.hpp"
#include "backend/protobuf/util.hpp"
#include "interfaces/common_objects/account.hpp"
#include "qry_responses.pb.h"

namespace shared_model {
  class Account final : public TrivialProto<Account, iroha::protocol::Account> {
   public:
    template <typename AccountType>
    explicit Account(AccountType &&account)
        : TrivialProto(std::forward<AccountType>(account)) {}

    Account(const Account &o) : Account(o.proto_) {}

    Account(Account &&o) noexcept : Account(std::move(o.proto_)) {}

    const types::AccountIdType &accountId() const override {
      return proto_->account_id();
    }

    const types::DomainIdType &domainId() const override {
      return proto_->domain_id();
    }

    types::QuorumType quorum() const override {
      return proto_->quorum();
    }

    const types::JsonType &jsonData() const override {
      return proto_->json_data();
    }
  };
}  // namespace shared_model

#endif  // IROHA_SHARED_MODEL_PROTO_ACCOUNT_HPP
