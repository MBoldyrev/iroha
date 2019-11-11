/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "interfaces/common_objects/signature.hpp"

#include "utils/string_builder.hpp"

using namespace shared_model;

Signature::Signature(const types::PubkeyType &key,
                     const Signature::SignedType &signed_data)
    : Signature([&]() -> TransportType {
        iroha::protocol::Signature signature;
        signature.set_public_key(key.hex());
        signature.set_signature(signed_data.hex());
        return signature;
      }()) {}

const PublicKeyType &Signature::publicKey() const override {
  return public_key_;
}

const SignedType &Signature::signedData() const override {
  return signed_;
}

bool Signature::operator==(const Signature &rhs) const {
  return publicKey() == rhs.publicKey();
}

std::string Signature::toString() const {
  return detail::PrettyStringBuilder()
      .init("Signature")
      .append("publicKey", publicKey().hex())
      .append("signedData", signedData().hex())
      .finalize();
}
