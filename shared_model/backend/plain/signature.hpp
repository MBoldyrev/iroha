/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_PLAIN_SIGNATURE_HPP
#define IROHA_SHARED_MODEL_PLAIN_SIGNATURE_HPP

#include "cryptography/public_key.hpp"
#include "cryptography/signed.hpp"
#include "interfaces/common_objects/signature.hpp"

namespace shared_model {

  namespace plain {

    class Signature final : public Signature {
     public:
      Signature(const SignedType &signedData, const PublicKeyType &publicKey);

      const Signature::PublicKeyType &publicKey() const override;

      const Signature::SignedType &signedData() const override;

     protected:
      Signature *clone() const override;

     private:
      const Signature::SignedType signed_data_;
      const Signature::PublicKeyType public_key_;
    };

  }  // namespace plain
}  // namespace shared_model

#endif  // IROHA_SHARED_MODEL_PLAIN_SIGNATURE_HPP
