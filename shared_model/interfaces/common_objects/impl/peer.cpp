/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "interfaces/common_objects/peer.hpp"

#include <boost/optional.hpp>

#include "cryptography/public_key.hpp"

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
