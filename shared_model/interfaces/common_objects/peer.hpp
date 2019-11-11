/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_PEER_HPP
#define IROHA_SHARED_MODEL_PEER_HPP

#include <boost/optional/optional.hpp>
#include "cryptography/hash.hpp"
#include "cryptography/public_key.hpp"
#include "interfaces/common_objects/trivial_proto.hpp"
#include "interfaces/common_objects/types.hpp"
#include "primitive.pb.h"

namespace shared_model {

  /**
   * Representation of a network participant (Iroha node).
   */
  class Peer : public TrivialProto<Peer, iroha::protocol::Peer> {
   public:
    template <typename T>
    explicit Peer(T &&peer) : TrivialProto(std::forward<T>(peer)) {}

    Peer(const types::AddressType &address,
         const types::PubkeyType &public_key,
         const boost::optional<types::TLSCertificateType> &tls_certificate =
             boost::none);

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
    const types::PubkeyType public_key_{
        crypto::Hash::fromHexString(proto_->peer_key())};
    boost::optional<types::TLSCertificateType> tls_certificate_{[this] {
      if (proto_->certificate_case()) {
        return proto_->tls_certificate();
      }
      return boost::none;
    }()};
  };
}  // namespace shared_model

#endif  // IROHA_SHARED_MODEL_PEER_HPP
