/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "backend/plain/peer.hpp"

using namespace shared_model;
using namespace shared_model::plain;

Peer::Peer(const types::AddressType &address,
           const types::PubkeyType &public_key,
           const boost::optional<types::TLSCertificateType> &tls_certificate)
    : address_(address),
      public_key_(public_key),
      tls_certificate_(tls_certificate) {}

const shared_model::types::AddressType &Peer::address() const {
  return address_;
}

const shared_model::types::PubkeyType &Peer::pubkey() const {
  return public_key_;
}

const boost::optional<shared_model::types::TLSCertificateType>
    &Peer::tlsCertificate() const {
  return tls_certificate_;
}
