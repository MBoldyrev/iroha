/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "interfaces/common_objects/peer.hpp"

#include "utils/string_builder.hpp"

using namespace shared_model;

Peer::Peer(const types::AddressType &address,
           const types::PubkeyType &public_key,
           const boost::optional<types::TLSCertificateType> &tls_certificate)
    : Peer([&]() -> TransportType {
        iroha::protocol::Peer peer;
        peer.set_address(address);
        peer.set_peer_key(public_key.hex());
        if (tls_certificate) {
          peer.set_tls_certificate(*tls_certificate);
        }
        return peer;
      }()) {}

const types::AddressType &Peer::address() const override {
  return proto_->address();
}

const boost::optional<types::TLSCertificateType> &Peer::tlsCertificate()
    const override {
  return tls_certificate_;
}

const types::PubkeyType &Peer::pubkey() const override {
  return public_key_;
}

std::string Peer::toString() const {
  return detail::PrettyStringBuilder()
      .init("Peer")
      .append("address", address())
      .append("pubkey", pubkey().toString())
      .append("tlsCertificate", bool(tlsCertificate()))
      .finalize();
}

bool Peer::operator==(const ModelType &rhs) const {
  return address() == rhs.address() and pubkey() == rhs.pubkey()
      and tlsCertificate() == rhs.tlsCertificate();
}
