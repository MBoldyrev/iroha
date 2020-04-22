/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_STRING_VIEW_TYPES_TO_STREAM_HPP
#define IROHA_SHARED_MODEL_STRING_VIEW_TYPES_TO_STREAM_HPP

#include "interfaces/common_objects/string_view_types.hpp"

#include <iostream>
#include <string_view>
#include <type_traits>

template <
    typename T,
    typename = std::enable_if_t<
        std::is_same_v<
            T,
            shared_model::interface::types::
                PublicKeyHexStringView> or std::is_same_v<T, shared_model::interface::types::SignedHexStringView>>>
inline std::ostream &operator<<(std::ostream &os, const T &o) {
  return os << std::string_view{o};
}

#endif
