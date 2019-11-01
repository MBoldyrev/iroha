/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_PROTO_PEER_HPP
#define IROHA_SHARED_MODEL_PROTO_PEER_HPP

#include "interfaces/common_objects/peer.hpp"

#include <boost/optional.hpp>

#include "cryptography/hash.hpp"
#include "cryptography/public_key.hpp"
#include "primitive.pb.h"
#include "utils/reference_holder.hpp"

namespace shared_model {
  class Peer final : public Peer {
   public:
    template <typename PeerType>
    explicit Peer(PeerType &&peer) : proto_(std::forward<PeerType>(peer)) {
      if (proto_->certificate_case()) {
        tls_certificate_ = proto_->tls_certificate();
      }
    }

    Peer(const Peer &o) : Peer(o.proto_) {}

    Peer(Peer &&o) noexcept : Peer(std::move(o.proto_)) {}

    const types::AddressType &address() const override {
      return proto_->address();
    }

    const boost::optional<types::TLSCertificateType> &tlsCertificate()
        const override {
      return tls_certificate_;
    }

    const types::PubkeyType &pubkey() const override {
      return public_key_;
    }

   private:
    detail::ReferenceHolder<iroha::protocol::Peer> proto_;
    const types::PubkeyType public_key_{
        crypto::Hash::fromHexString(proto_->peer_key())};
    boost::optional<std::string> tls_certificate_;
  };
}  // namespace shared_model
#endif  // IROHA_SHARED_MODEL_PROTO_PEER_HPP
