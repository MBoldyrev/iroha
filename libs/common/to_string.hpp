/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_LIBS_TO_STRING_HPP
#define IROHA_LIBS_TO_STRING_HPP

#include <functional>
#include <ios>
#include <iostream>
#include <memory>
#include <optional>
#include <ostream>
#include <sstream>
#include <string>
#include <string_view>

#include <boost/optional.hpp>
#include <boost/optional/optional_io.hpp>
#include <type_traits>

#include "interfaces/common_objects/string_view_types_to_stream.hpp"

namespace iroha {
  namespace to_string {
    namespace detail {
      const std::string kBeginBlockMarker = "[";
      const std::string kEndBlockMarker = "]";
      const std::string kSingleFieldsSeparator = ", ";
      const std::string kNotSet = "(not set)";

      /// Print pointers and optionals.
      template <typename T>
      inline std::ostream &outputDereferenced(std::ostream &os, const T &o);
    }  // namespace detail
  }    // namespace to_string
}  // namespace iroha

template <typename T>
inline auto operator<<(std::ostream &os, const T &o)
    -> decltype(os << o.toString()) {
  return os << o.toString();
}

template <typename... T>
inline auto operator<<(std::ostream &os, const std::reference_wrapper<T...> &o)
    -> decltype(os << o.get()) {
  return os << o.get();
}

template <typename... T>
inline auto operator<<(std::ostream &os, const std::unique_ptr<T...> &o)
    -> decltype(os << *o) {
  return iroha::to_string::detail::outputDereferenced(os, o);
}

template <typename... T>
inline auto operator<<(std::ostream &os, const std::shared_ptr<T...> &o)
    -> decltype(os << *o) {
  return iroha::to_string::detail::outputDereferenced(os, o);
}

template <typename... T>
inline auto operator<<(std::ostream &os, const std::optional<T...> &o)
    -> decltype(os << *o) {
  return iroha::to_string::detail::outputDereferenced(os, o);
}

inline std::ostream &operator<<(std::ostream &os, const std::nullopt_t &) {
  return iroha::to_string::detail::outputDereferenced(
      os, static_cast<int *>(nullptr));
}

/// Print a plain collection.
template <typename T,
          typename = std::enable_if_t<
              not std::is_same_v<std::decay_t<typename T::value_type>, char>>>
inline auto operator<<(std::ostream &os, const T &c)
    -> decltype(os << *c.begin()) {
  os << iroha::to_string::detail::kBeginBlockMarker;
  bool need_field_separator = false;
  for (auto &o : c) {
    if (need_field_separator) {
      os << iroha::to_string::detail::kSingleFieldsSeparator;
    }
    os << o;
    need_field_separator = true;
  }
  os << iroha::to_string::detail::kEndBlockMarker;
  return os;
}

namespace iroha {
  namespace to_string {
    namespace detail {
      template <typename T>
      inline std::ostream &outputDereferenced(std::ostream &os, const T &o) {
        return o ? (os << *o) : (os << kNotSet);
      }
    }  // namespace detail
    template <typename T,
              typename = decltype(std::declval<std::ostream &>()
                                  << std::declval<T>())>
    inline std::string toString(const T &o) {
      std::stringstream ss;
      ss << std::boolalpha << o;
      return ss.str();
    }
  }  // namespace to_string
}  // namespace iroha

#endif
