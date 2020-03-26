/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_LOGGER_LOGGER_HPP
#define IROHA_LOGGER_LOGGER_HPP

#include "logger/logger_fwd.hpp"

#include <string>

#include <fmt/core.h>
#include <fmt/format.h>
// Windows includes transitively included by format.h define interface as
// struct, leading to compilation issues
#undef interface

#include "common/to_string.hpp"

namespace iroha {
  namespace utility_service {
    namespace proto {
      class Status;
    }
  }  // namespace utility_service
}  // namespace iroha

namespace logger {
  namespace detail {
    template <typename T>
    class FmtSupportTypeChecker {
     private:
      typedef char YesType[1];
      typedef char NoType[2];

      template <typename C>
      static YesType &test(decltype(
          std::declval<::fmt::internal::arg_mapper<::fmt::format_context>>()
              .map(std::declval<C>())));
      template <typename C>
      static NoType &test(...);

     public:
      enum { value = sizeof(test<T>(0)) == sizeof(YesType) };
    };
  }  // namespace detail
  template <typename T>
  constexpr bool kIsFmtInternalSupported =  // true;
      detail::FmtSupportTypeChecker<T>::value;
}  // namespace logger

namespace fmt {
  /// Allows to directly log objects, which are not supported by libfmt
  /// natively, using iroha::to_string
  template <typename T>
  struct formatter<
      T,
      std::enable_if_t<!::logger::kIsFmtInternalSupported<T>, char>> {
    template <typename ParseContext>
    auto parse(ParseContext &ctx) -> decltype(ctx.begin()) {
      return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const T &val, FormatContext &ctx) -> decltype(ctx.out()) {
      return format_to(ctx.out(), "{}", ::iroha::to_string::toString(val));
    }
  };
}  // namespace fmt

namespace logger {

  enum class LogLevel;

  extern const LogLevel kDefaultLogLevel;

  /// Log levels
  enum class LogLevel {
    kTrace,
    kDebug,
    kInfo,
    kWarn,
    kError,
    kCritical,
  };

  class Logger {
   public:
    using Level = LogLevel;

    virtual ~Logger() = default;

    // --- Logging functions ---

    template <typename... Args>
    void trace(const std::string &format, const Args &... args) const {
      log(LogLevel::kTrace, format, args...);
    }

    template <typename... Args>
    void debug(const std::string &format, const Args &... args) const {
      log(LogLevel::kDebug, format, args...);
    }

    template <typename... Args>
    void info(const std::string &format, const Args &... args) const {
      log(LogLevel::kInfo, format, args...);
    }

    template <typename... Args>
    void warn(const std::string &format, const Args &... args) const {
      log(LogLevel::kWarn, format, args...);
    }

    template <typename... Args>
    void error(const std::string &format, const Args &... args) const {
      log(LogLevel::kError, format, args...);
    }

    template <typename... Args>
    void critical(const std::string &format, const Args &... args) const {
      log(LogLevel::kCritical, format, args...);
    }

    template <typename... Args>
    void log(Level level,
             const std::string &format,
             const Args &... args) const {
      if (shouldLog(level)) {
        try {
          logInternal(level, fmt::format(format, args...));
        } catch (const std::exception &error) {
          std::string error_msg("Exception was thrown while logging: ");
          logInternal(LogLevel::kError, error_msg.append(error.what()));
        }
      }
    }

   protected:
    virtual void logInternal(Level level, const std::string &s) const = 0;

    /// Whether the configured logging level is at least as verbose as the
    /// one given in parameter.
    virtual bool shouldLog(Level level) const = 0;
  };

  /**
   * Convert bool value to human readable string repr
   * @param value value for transformation
   * @return "true" or "false"
   */
  std::string boolRepr(bool value);

}  // namespace logger

#endif  // IROHA_LOGGER_LOGGER_HPP
