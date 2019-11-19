/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_MODEL_PRIMITIVE_HPP
#define IROHA_MODEL_PRIMITIVE_HPP

#include <ciso646>

#include "utils/string_builder.hpp"

namespace shared_model {
  /**
   * ModelPrimitive is a base class of whole domain objects in system.
   * This class required for guarantee consistent interface on all shared
   * model objects.
   * @tparam Model - your new style model
   */
  template <typename Model>
  class ModelPrimitive {
   public:
    /**
     * Reference for model type.
     */
    using ModelType = Model;

    /**
     * Make string developer representation of object
     * @return string with internal state of object
     */
    std::string toString() const {
      return detail::PrettyStringBuilder()
          .init("Primitive")
          .append("address", std::to_string(reinterpret_cast<uint64_t>(this)))
          .finalize();
    }

    bool operator==(const ModelType &rhs) const;

    bool operator!=(const ModelType &rhs) const {
      return not(*this == rhs);
    }

    ~ModelPrimitive() = default;
  };
}  // namespace shared_model
#endif  // IROHA_MODEL_PRIMITIVE_HPP
