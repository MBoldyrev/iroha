/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SOCI_SESSION_WRAPPER_HPP
#define IROHA_SOCI_SESSION_WRAPPER_HPP

#include <soci/soci.h>

#include <type_traits>

#include "obj_counter.hpp"

namespace iroha {

  namespace ametsuchi {
    /**
     * Public interface for queries on blocks and transactions
     */
    class SociSession : public soci::session, public ObjCounter<SociSession> {
     public:
      template <typename... Args,
                typename = std::enable_if_t<
                    std::is_constructible<soci::session, Args...>::value>>
      SociSession(Args &&... args)
          : soci::session(std::forward<Args>(args)...) {}

      SociSession(const SociSession &) = default;
      SociSession(SociSession &&) = default;
    };
  }  // namespace ametsuchi
}  // namespace iroha

#endif  // IROHA_SOCI_SESSION_WRAPPER_HPP
