/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "interfaces/common_objects/account.hpp"

#include "utils/string_builder.hpp"

using namespace shared_model;

Account::Account(const types::AccountIdType &account_id,
                 const types::DomainIdType &domain_id,
                 types::QuorumType quorum,
                 const types::JsonType &jsonData)
    : Account([&]() -> TransportType {
        iroha::protocol::Account account;
        account.set_account_id(account_id);
        account.set_domain_id(domain_id);
        account.set_quorum(quorum);
        account.set_json_data(jsonData);
        return account;
      }()) {}

const types::AccountIdType &Account::accountId() const override {
  return proto_->account_id();
}

const types::DomainIdType &Account::domainId() const override {
  return proto_->domain_id();
}

types::QuorumType Account::quorum() const override {
  return proto_->quorum();
}

const types::JsonType &Account::jsonData() const override {
  return proto_->json_data();
}

std::string Account::toString() const override {
  return detail::PrettyStringBuilder()
      .init("Account")
      .append("accountId", accountId())
      .append("domainId", domainId())
      .append("quorum", std::to_string(quorum()))
      .append("json", jsonData())
      .finalize();
}

bool Account::operator==(const Account &rhs) const override {
  return accountId() == rhs.accountId() and domainId() == rhs.domainId()
      and quorum() == rhs.quorum() and jsonData() == rhs.jsonData();
}
