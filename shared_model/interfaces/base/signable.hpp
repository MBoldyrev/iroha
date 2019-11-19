/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SIGNABLE_HPP
#define IROHA_SIGNABLE_HPP

#include "interfaces/base/model_primitive.hpp"

#include <boost/functional/hash.hpp>
#include <boost/optional.hpp>
#include <unordered_set>
#include "cryptography/default_hash_provider.hpp"
#include "interfaces/common_objects/range_types.hpp"
#include "interfaces/common_objects/signature.hpp"
#include "interfaces/common_objects/types.hpp"
#include "utils/string_builder.hpp"

namespace shared_model {

  namespace crypto {
    class Signed;
    class PublicKey;
  }  // namespace crypto

  /**
   * Interface provides signatures and adds them to model object
   * @tparam Model - your model
   */
  template <typename Model,
            typename HashProvider = shared_model::crypto::Sha3_256>
  class Signable : public ModelPrimitive<Model> {
   public:
    /**
     * @return attached signatures
     */
    types::SignatureRangeType signatures() const;

    /**
     * Attach signature to object
     * @param signature - signature object for insertion
     * @return true, if signature was added
     */
    bool addSignature(const crypto::Signed &signed_blob,
                      const crypto::PublicKey &public_key);

    /**
     * @return time of creation
     */
    types::TimestampType createdTime() const;

    /**
     * @return object payload (everything except signatures)
     */
    const types::BlobType &payload() const;

    /**
     * @return blob representation of object include signatures
     */
    const types::BlobType &blob() const;

    /**
     * Provides comparison based on equality of objects and signatures.
     * @param rhs - another model object
     * @return true, if objects totally equal
     */
    bool operator==(const Model &rhs) const override {
      return equalsByValue(rhs)
          // is_permutation consumes ~O(N^2)
          and std::is_permutation(signatures().begin(),
                                  signatures().end(),
                                  rhs.signatures().begin());
    }

    /**
     * Provides comaprison based on equality objects only
     * @param rhs - another model object
     * @return true, if hashes of objects are equal
     */
    bool equalsByValue(const Model &rhs) const {
      return this->hash() == rhs.hash();
    }

    const types::HashType &hash() const;

    // ------------------------| Primitive override |-------------------------

    std::string toString() const override {
      return detail::PrettyStringBuilder()
          .init("Signable")
          .append("created_time", std::to_string(createdTime()))
          .appendAll(signatures(),
                     [](auto &signature) { return signature.toString(); })
          .finalize();
    }

   protected:
    /**
     * Type of set of signatures
     *
     * Note: we can't use const SignatureType due to unordered_set
     * limitations: it requires to have write access for elements for some
     * internal operations.
     */

   protected:
    class SignatureSetTypeOps {
     public:
      /**
       * @param sig is item to find hash from
       * @return calculated hash of public key
       */
      template <typename T>
      size_t operator()(const T &sig) const {
        return std::hash<std::string>{}(sig.publicKey().hex());
      }

      /**
       * Function for set elements uniqueness by public key
       * @param lhs
       * @param rhs
       * @return true, if public keys are the same
       */
      template <typename T>
      bool operator()(const T &lhs, const T &rhs) const {
        return lhs.publicKey() == rhs.publicKey();
      }
    };

    template <typename T>
    using SignatureSetType =
        std::unordered_set<T, SignatureSetTypeOps, SignatureSetTypeOps>;

   protected:
    static auto makeHash(const types::BlobType &payload) {
      return HashProvider::makeHash(payload);
    }
  };

}  // namespace shared_model

#endif  // IROHA_SIGNABLE_HPP
