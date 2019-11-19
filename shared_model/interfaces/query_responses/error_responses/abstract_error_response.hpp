/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_ABSTRACT_ERROR_RESPONSE_HPP
#define IROHA_ABSTRACT_ERROR_RESPONSE_HPP

#include "interfaces/base/model_primitive.hpp"

#include "utils/string_builder.hpp"

namespace shared_model {
  /**
   * Abstract error response
   * @tparam Model - concrete model error response
   */
  template <typename Model>
  class AbstractErrorResponse : public ModelPrimitive<Model> {
   private:
    /**
     * @return string representation of error reason
     */
    std::string reason() const;

   public:
    // ------------------------| Primitive override |-------------------------

    std::string toString() const override {
      return detail::PrettyStringBuilder().init(reason()).finalize();
    }

    bool operator==(const Model &rhs) const override {
      return true;
    }
  };
}  // namespace shared_model

#endif  // IROHA_ABSTRACT_ERROR_RESPONSE_HPP
