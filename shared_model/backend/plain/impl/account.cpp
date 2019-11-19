/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "backend/plain/account.hpp"

shared_model::plain::Account::Account(
    const shared_model::types::AccountIdType &account_id,
    const shared_model::types::DomainIdType &domain_id,
    shared_model::types::QuorumType quorum,
    const shared_model::types::JsonType &json_data)
    : account_id_(account_id),
      domain_id_(domain_id),
      quorum_(quorum),
      json_data_(json_data) {}

const shared_model::types::AccountIdType &
shared_model::plain::Account::accountId() const {
  return account_id_;
}
const shared_model::types::DomainIdType &
shared_model::plain::Account::domainId() const {
  return domain_id_;
}
shared_model::types::QuorumType shared_model::plain::Account::quorum() const {
  return quorum_;
}
const shared_model::types::JsonType &shared_model::plain::Account::jsonData()
    const {
  return json_data_;
}
