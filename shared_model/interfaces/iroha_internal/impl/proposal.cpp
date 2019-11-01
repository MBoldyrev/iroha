/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "interfaces/proposal.hpp"

#include "interfaces/iroha_internal/util.hpp"
#include "interfaces/transaction.hpp"

using namespace types;

struct Proposal::Impl {
  explicit Impl(TransportType &&ref) : proto_(std::move(ref)) {}

  explicit Impl(const TransportType &ref) : proto_(ref) {}

  TransportType proto_;

  const std::vector<Transaction> transactions_{[this] {
    return std::vector<Transaction>(proto_.mutable_transactions()->begin(),
                                    proto_.mutable_transactions()->end());
  }()};

  types::BlobType blob_{[this] { return util::makeBlob(proto_); }()};

  const types::HashType hash_{
      [this] { return crypto::DefaultHashProvider::makeHash(blob_); }()};
};

Proposal::Proposal(Proposal &&o) noexcept = default;

Proposal::Proposal(const TransportType &ref) {
  impl_ = std::make_unique<Proposal::Impl>(ref);
}

Proposal::Proposal(TransportType &&ref) {
  impl_ = std::make_unique<Proposal::Impl>(std::move(ref));
}

TransactionsCollectionType Proposal::transactions() const {
  return impl_->transactions_;
}

TimestampType Proposal::createdTime() const {
  return impl_->proto_.created_time();
}

HeightType Proposal::height() const {
  return impl_->proto_.height();
}

const types::BlobType &Proposal::blob() const {
  return impl_->blob_;
}

const Proposal::TransportType &Proposal::getTransport() const {
  return impl_->proto_;
}

const types::HashType &Proposal::hash() const {
  return impl_->hash_;
}

Proposal::~Proposal() = default;
