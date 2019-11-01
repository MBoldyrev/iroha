/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "interfaces/block.hpp"

#include <boost/range/adaptors.hpp>
#include "common/byteutils.hpp"
#include "interfaces/common_objects/signature.hpp"
#include "interfaces/transaction.hpp"
#include "interfaces/util.hpp"
#include "utils/string_builder.hpp"

using namespace shared_model;

struct Block::Impl {
  explicit Impl(TransportType &&ref) : proto_(std::move(ref)) {}
  explicit Impl(const TransportType &ref) : proto_(ref) {}
  Impl(Impl &&o) noexcept = delete;
  Impl &operator=(Impl &&o) noexcept = delete;

  TransportType proto_;
  iroha::protocol::Block_v1::Payload &payload_{*proto_.mutable_payload()};

  std::vector<Transaction> transactions_{[this] {
    return std::vector<Transaction>(payload_.mutable_transactions()->begin(),
                                    payload_.mutable_transactions()->end());
  }()};

  types::BlobType blob_{[this] { return makeBlob(proto_); }()};

  types::HashType prev_hash_{[this] {
    return types::HashType(
        crypto::Hash::fromHexString(proto_.payload().prev_block_hash()));
  }()};

  SignatureSetType<Signature> signatures_{[this] {
    auto signatures = *proto_.mutable_signatures()
        | boost::adaptors::transformed([](auto &x) { return Signature(x); });
    return SignatureSetType<Signature>(signatures.begin(), signatures.end());
  }()};

  std::vector<types::HashType> rejected_transactions_hashes_{[this] {
    std::vector<types::HashType> hashes;
    for (const auto &hash : *payload_.mutable_rejected_transactions_hashes()) {
      hashes.emplace_back(crypto::Hash::fromHexString(hash));
    }
    return hashes;
  }()};

  types::BlobType payload_blob_{[this] { return makeBlob(payload_); }()};

  types::HashType hash_ = makeHash(payload_blob_);
};

Block::Block(Block &&o) noexcept = default;

Block::Block(const TransportType &ref) {
  impl_ = std::make_unique<Block::Impl>(ref);
}

Block::Block(TransportType &&ref) {
  impl_ = std::make_unique<Block::Impl>(std::move(ref));
}

types::TransactionsCollectionType Block::transactions() const {
  return impl_->transactions_;
}

types::HeightType Block::height() const {
  return impl_->payload_.height();
}

const types::HashType &Block::prevHash() const {
  return impl_->prev_hash_;
}

const types::BlobType &Block::blob() const {
  return impl_->blob_;
}

types::SignatureRangeType Block::signatures() const {
  return impl_->signatures_;
}

bool Block::addSignature(const crypto::Signed &signed_blob,
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

  auto sig = impl_->proto_.add_signatures();
  sig->set_signature(signed_blob.hex());
  sig->set_public_key(public_key.hex());

  impl_->signatures_ = [this] {
    auto signatures = *impl_->proto_.mutable_signatures()
        | boost::adaptors::transformed([](auto &x) { return Signature(x); });
    return SignatureSetType<Signature>(signatures.begin(), signatures.end());
  }();
  return true;
}

const types::HashType &Block::hash() const {
  return impl_->hash_;
}

types::TimestampType Block::createdTime() const {
  return impl_->payload_.created_time();
}

types::TransactionsNumberType Block::txsNumber() const {
  return impl_->payload_.tx_number();
}

types::HashCollectionType Block::rejected_transactions_hashes() const {
  return impl_->rejected_transactions_hashes_;
}

const types::BlobType &Block::payload() const {
  return impl_->payload_blob_;
}

const iroha::protocol::Block_v1 &Block::getTransport() const {
  return impl_->proto_;
}

Block::ModelType *Block::clone() const {
  return new Block(impl_->proto_);
}

Block::~Block() = default;

std::string Block::toString() const {
  return detail::PrettyStringBuilder()
      .init("Block")
      .append("hash", hash().hex())
      .append("height", std::to_string(height()))
      .append("prevHash", prevHash().hex())
      .append("txsNumber", std::to_string(txsNumber()))
      .append("createdtime", std::to_string(createdTime()))
      .append("transactions")
      .appendAll(transactions(), [](auto &tx) { return tx.toString(); })
      .append("signatures")
      .appendAll(signatures(), [](auto &sig) { return sig.toString(); })
      .appendAll("rejected transactions",
                 rejected_transactions_hashes(),
                 [](auto &hash) { return hash.hex(); })
      .finalize();
}
