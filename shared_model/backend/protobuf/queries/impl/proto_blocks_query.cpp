/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "backend/protobuf/queries/proto_blocks_query.hpp"

#include "backend/protobuf/util.hpp"

BlocksQuery::BlocksQuery(const TransportType &query)
    : BlocksQuery(TransportType(query)) {}

BlocksQuery::BlocksQuery(TransportType &&query)
    : proto_{std::move(query)},
      blob_{makeBlob(proto_)},
      payload_{makeBlob(proto_.meta())},
      signatures_{[this] {
        SignatureSetType<Signature> set;
        if (proto_.has_signature()) {
          set.emplace(*proto_.mutable_signature());
        }
        return set;
      }()},
      hash_(makeHash(payload_)) {}

const types::AccountIdType &BlocksQuery::creatorAccountId() const {
  return proto_.meta().creator_account_id();
}

types::CounterType BlocksQuery::queryCounter() const {
  return proto_.meta().query_counter();
}

const types::BlobType &BlocksQuery::blob() const {
  return blob_;
}

const types::BlobType &BlocksQuery::payload() const {
  return payload_;
}

types::SignatureRangeType BlocksQuery::signatures() const {
  return signatures_;
}

bool BlocksQuery::addSignature(const crypto::Signed &signed_blob,
                               const crypto::PublicKey &public_key) {
  if (proto_.has_signature()) {
    return false;
  }

  auto sig = proto_.mutable_signature();
  sig->set_signature(signed_blob.hex());
  sig->set_public_key(public_key.hex());
  // TODO: nickaleks IR-120 12.12.2018 remove set
  signatures_.emplace(*proto_.mutable_signature());
  return true;
}

const types::HashType &BlocksQuery::hash() const {
  return hash_;
}

types::TimestampType BlocksQuery::createdTime() const {
  return proto_.meta().created_time();
}

const BlocksQuery::TransportType &BlocksQuery::getTransport() const {
  return proto_;
}
