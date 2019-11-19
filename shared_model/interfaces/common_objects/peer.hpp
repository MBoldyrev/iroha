/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_PEER_HPP
#define IROHA_SHARED_MODEL_PEER_HPP

#include "interfaces/base/model_primitive.hpp"

#include <boost/optional.hpp>

#include "backend/protobuf/util.hpp"
#include "cryptography/hash.hpp"
#include "cryptography/public_key.hpp"
#include "interfaces/common_objects/types.hpp"
#include "primitive.pb.h"
#include "utils/reference_holder.hpp"

#include <boost/optional/optional_fwd.hpp>

namespace shared_model {

  /**
   * Representation of a network participant.
   */
  class Peer : public ModelPrimitive<Peer> {
   public:
    template <typename PeerType>
    explicit Peer(PeerType &&peer) : proto_(std::forward<PeerType>(peer)) {
      if (proto_->certificate_case()) {
        tls_certificate_ = proto_->tls_certificate();
      }
    }

    Peer(const Peer &o) : Peer(o.proto_) {}

    Peer(Peer &&o) noexcept : Peer(std::move(o.proto_)) {}

    /**
     * @return Peer address, for fetching data
     */
    const types::AddressType &address() const;

    /**
     * @return Peer TLS certficate
     */
    const boost::optional<types::TLSCertificateType> &tlsCertificate() const;

    /**
     * @return Public key, for fetching data
     */
    const types::PubkeyType &pubkey() const;

    std::string toString() const override;

    bool operator==(const ModelType &rhs) const override;

   private:
    detail::ReferenceHolder<iroha::protocol::Peer> proto_;
    const types::PubkeyType public_key_{
        crypto::Hash::fromHexString(proto_->peer_key())};
    boost::optional<std::string> tls_certificate_;
  };
}  // namespace shared_model

#endif  // IROHA_SHARED_MODEL_PEER_HPP
