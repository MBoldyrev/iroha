/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_PEER_HPP
#define IROHA_SHARED_MODEL_PEER_HPP

#include "interfaces/base/model_primitive.hpp"
#include "interfaces/common_objects/types.hpp"

#include <boost/optional/optional_fwd.hpp>

namespace shared_model {

  /**
   * Representation of a network participant.
   */
  class Peer : public ModelPrimitive<Peer> {
   public:
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
  };
}  // namespace shared_model

#endif  // IROHA_SHARED_MODEL_PEER_HPP
