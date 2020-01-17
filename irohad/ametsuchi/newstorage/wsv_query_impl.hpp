/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_WSV_QUERY_IMPL_HPP
#define IROHA_WSV_QUERY_IMPL_HPP

#include "ametsuchi/wsv_query.hpp"
#include "logger/logger_fwd.hpp"

namespace iroha {
  namespace newstorage {

  class ImmutableWsv;

  class WsvQueryImpl : public ametsuchi::WsvQuery {
     public:
      WsvQueryImpl(ImmutableWsv &wsv, logger::LoggerPtr log);

      boost::optional<std::vector<shared_model::interface::types::PubkeyType>>
      getSignatories(const shared_model::interface::types::AccountIdType
                         &account_id) override;

      boost::optional<
          std::vector<std::shared_ptr<shared_model::interface::Peer>>>
      getPeers() override;

     private:

      ImmutableWsv &wsv_;
      logger::LoggerPtr log_;
    };
  }  // namespace newstorage
}  // namespace iroha

#endif  // IROHA_WSV_QUERY_IMPL_HPP