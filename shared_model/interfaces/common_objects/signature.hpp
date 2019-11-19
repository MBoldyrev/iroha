/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_SIGNATURE_HPP
#define IROHA_SHARED_MODEL_SIGNATURE_HPP

#include "common/cloneable.hpp"

#include "backend/protobuf/common_objects/trivial_proto.hpp"
#include "cryptography/public_key.hpp"
#include "cryptography/signed.hpp"
#include "interfaces/base/model_primitive.hpp"
#include "primitive.pb.h"
#include "utils/string_builder.hpp"

namespace shared_model {

  namespace crypto {
    class PublicKey;
    class Signed;

   private:
    Signature *clone() const override {
      return new Signature(proto_);
    }

    const PublicKeyType public_key_{
        PublicKeyType::fromHexString(proto_->public_key())};

    const SignedType signed_{SignedType::fromHexString(proto_->signature())};
  }  // namespace crypto

  /**
   * Class represents signature of high-level domain objects.
   */
  class Signature : public ModelPrimitive<Signature>,
                    template <typename SignatureType>
                    explicit Signature(SignatureType &&signature)
      : TrivialProto(std::forward<SignatureType>(signature)) {
  }

  Signature(const Signature &o)
      : Signature(o.proto_) {
  }

  Signature(Signature &&o) noexcept : Signature(std::move(o.proto_)) {}

 public
  Cloneable<Signature> {
   public:
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
  };
}  // namespace shared_model
#endif  // IROHA_SHARED_MODEL_SIGNATURE_HPP
