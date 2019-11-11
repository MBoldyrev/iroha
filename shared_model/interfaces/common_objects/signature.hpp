/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_SIGNATURE_HPP
#define IROHA_SHARED_MODEL_SIGNATURE_HPP

#include "cryptography/public_key.hpp"
#include "cryptography/signed.hpp"
#include "interfaces/common_objects/trivial_proto.hpp"
#include "primitive.pb.h"

namespace shared_model {

  /**
   * Class represents signature of high-level domain objects.
   */
  class Signature : public TrivialProto<Signature, iroha::protocol::Signature> {
   public:
    template <typename T>
    explicit Signature(T &&signature)
        : TrivialProto(std::forward<T>(signature)) {}

    Signature(const types::PubkeyType &key,
              const Signature::SignedType &signed_data);

    /**
     * Type of public key
     */
    using PublicKeyType = crypto::PublicKey;

    /**
     * @return public key of signatory
     */
    const PublicKeyType &publicKey() const;

    /**
     * Type of signed data
     */
    using SignedType = crypto::Signed;

    /**
     * @return signed data
     */
    const SignedType &signedData() const;

    bool operator==(const Signature &rhs) const override;

    std::string toString() const override;

   private:
    const PublicKeyType public_key_{
        PublicKeyType::fromHexString(proto_->public_key())};

    const SignedType signed_{SignedType::fromHexString(proto_->signature())};
  };
}  // namespace shared_model
#endif  // IROHA_SHARED_MODEL_SIGNATURE_HPP
