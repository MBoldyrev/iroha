/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_PLAIN_PEER_HPP
#define IROHA_SHARED_MODEL_PLAIN_PEER_HPP

#include "cryptography/public_key.hpp"
#include "interfaces/common_objects/peer.hpp"

#include <boost/optional.hpp>

namespace shared_model {
  namespace plain {

    class Peer final : public Peer {
     public:
      Peer(const types::AddressType &address,
           const types::PubkeyType &public_key,
           const boost::optional<types::TLSCertificateType> &tls_certificate);

      const types::AddressType &address() const override;

      const types::PubkeyType &pubkey() const override;

      const boost::optional<types::TLSCertificateType> &tlsCertificate()
          const override;

     private:
      const types::AddressType address_;
      const types::PubkeyType public_key_;
      const boost::optional<types::TLSCertificateType> tls_certificate_;
    };

  }  // namespace plain
}  // namespace shared_model

#endif  // IROHA_SHARED_MODEL_PLAIN_PEER_HPP
