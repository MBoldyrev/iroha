/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "backend/protobuf/query_responses/proto_signatories_response.hpp"

#include <boost/range/adaptor/transformed.hpp>
#include "cryptography/hash.hpp"

SignatoriesResponse::SignatoriesResponse(
    iroha::protocol::QueryResponse &query_response)
    : signatories_response_{query_response.signatories_response()},
      keys_{boost::copy_range<types::PublicKeyCollectionType>(
          signatories_response_.keys()
          | boost::adaptors::transformed([](const auto &key) {
              return crypto::PublicKey{crypto::PublicKey::fromHexString(key)};
            }))} {}

const types::PublicKeyCollectionType &SignatoriesResponse::keys() const {
  return keys_;
}
