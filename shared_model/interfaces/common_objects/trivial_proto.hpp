/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_TRIVIAL_PROTO_HPP
#define IROHA_SHARED_MODEL_TRIVIAL_PROTO_HPP

#include "interfaces/base/model_primitive.hpp"
#include "utils/reference_holder.hpp"

namespace shared_model {
  /**
   * Simple generic class for handling proto objects
   * @tparam Iface is interface to inherit from
   * @tparam Proto is protobuf container
   */
  template <typename Iface, typename Proto>
  class TrivialProto : public ModelPrimitive<Iface> {
   public:
    using TransportType = Proto;

    /**
     * @tparm ProtoLoader generic param so it can be handled
     *                    in the load for the boost::variant
     */
    template <typename ProtoLoader>
    explicit TrivialProto(ProtoLoader &&ref)
        : proto_(std::forward<ProtoLoader>(ref)) {}

    const Proto &getTransport() const {
      return *proto_;
    }

   protected:
    detail::ReferenceHolder<Proto> proto_;
  };

}  // namespace shared_model

#endif  // IROHA_SHARED_MODEL_TRIVIAL_PROTO_HPP
