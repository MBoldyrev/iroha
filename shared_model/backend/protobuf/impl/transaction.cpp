/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "backend/protobuf/transaction.hpp"

#include <boost/range/adaptor/transformed.hpp>
#include "backend/protobuf/batch_meta.hpp"
#include "backend/protobuf/commands/proto_command.hpp"
#include "backend/protobuf/common_objects/signature.hpp"
#include "backend/protobuf/util.hpp"
#include "utils/reference_holder.hpp"

struct Transaction::Impl {
  explicit Impl(const TransportType &ref) : proto_{ref} {}

  explicit Impl(TransportType &&ref) : proto_{std::move(ref)} {}

  explicit Impl(TransportType &ref) : proto_{ref} {}

  detail::ReferenceHolder<TransportType> proto_;

  iroha::protocol::Transaction::Payload &payload_{*proto_->mutable_payload()};

  iroha::protocol::Transaction::Payload::ReducedPayload &reduced_payload_{
      *proto_->mutable_payload()->mutable_reduced_payload()};

  types::BlobType blob_{[this] { return makeBlob(*proto_); }()};

  types::BlobType payload_blob_{[this] { return makeBlob(payload_); }()};

  types::BlobType reduced_payload_blob_{
      [this] { return makeBlob(reduced_payload_); }()};

  types::HashType reduced_hash_{makeHash(reduced_payload_blob_)};

  std::vector<Command> commands_{reduced_payload_.mutable_commands()->begin(),
                                 reduced_payload_.mutable_commands()->end()};

  boost::optional<std::shared_ptr<BatchMeta>> meta_{
      [this]() -> boost::optional<std::shared_ptr<BatchMeta>> {
        if (payload_.has_batch()) {
          std::shared_ptr<BatchMeta> b =
              std::make_shared<BatchMeta>(*payload_.mutable_batch());
          return b;
        }
        return boost::none;
      }()};

  SignatureSetType<Signature> signatures_{[this] {
    auto signatures = *proto_->mutable_signatures()
        | boost::adaptors::transformed([](auto &x) { return Signature(x); });
    return SignatureSetType<Signature>(signatures.begin(), signatures.end());
  }()};

  types::HashType hash_{makeHash(payload_blob_)};
};

Transaction::Transaction(const TransportType &transaction) {
  impl_ = std::make_unique<Transaction::Impl>(transaction);
}

Transaction::Transaction(TransportType &&transaction) {
  impl_ = std::make_unique<Transaction::Impl>(std::move(transaction));
}

Transaction::Transaction(TransportType &transaction) {
  impl_ = std::make_unique<Transaction::Impl>(transaction);
}

// TODO [IR-1866] Akvinikym 13.11.18: remove the copy ctor and fix fallen
// tests
Transaction::Transaction(const Transaction &transaction)
    : Transaction(
          static_cast<const TransportType &>(*transaction.impl_->proto_)) {}

Transaction::Transaction(Transaction &&transaction) noexcept = default;

Transaction::~Transaction() = default;

const types::AccountIdType &Transaction::creatorAccountId() const {
  return impl_->reduced_payload_.creator_account_id();
}

Transaction::CommandsType Transaction::commands() const {
  return impl_->commands_;
}

const types::BlobType &Transaction::blob() const {
  return impl_->blob_;
}

const types::BlobType &Transaction::payload() const {
  return impl_->payload_blob_;
}

const types::BlobType &Transaction::reducedPayload() const {
  return impl_->reduced_payload_blob_;
}

types::SignatureRangeType Transaction::signatures() const {
  return impl_->signatures_;
}

const types::HashType &Transaction::reducedHash() const {
  return impl_->reduced_hash_;
}

bool Transaction::addSignature(const crypto::Signed &signed_blob,
                               const crypto::PublicKey &public_key) {
  // if already has such signature
  if (std::find_if(impl_->signatures_.begin(),
                   impl_->signatures_.end(),
                   [&public_key](const auto &signature) {
                     return signature.publicKey() == public_key;
                   })
      != impl_->signatures_.end()) {
    return false;
  }

  auto sig = impl_->proto_->add_signatures();
  sig->set_signature(signed_blob.hex());
  sig->set_public_key(public_key.hex());

  impl_->signatures_ = [this] {
    auto signatures = *impl_->proto_->mutable_signatures()
        | boost::adaptors::transformed([](auto &x) { return Signature(x); });
    return SignatureSetType<Signature>(signatures.begin(), signatures.end());
  }();

  return true;
}

const types::HashType &Transaction::hash() const {
  return impl_->hash_;
}

const Transaction::TransportType &Transaction::getTransport() const {
  return *impl_->proto_;
}

types::TimestampType Transaction::createdTime() const {
  return impl_->reduced_payload_.created_time();
}

types::QuorumType Transaction::quorum() const {
  return impl_->reduced_payload_.quorum();
}

boost::optional<std::shared_ptr<BatchMeta>> Transaction::batchMeta() const {
  return impl_->meta_;
}

Transaction::ModelType *Transaction::clone() const {
  return new Transaction(TransportType(*impl_->proto_));
}
