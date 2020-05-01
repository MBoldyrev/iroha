/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_PROTO_MODEL_ENGINE_RECEIPT_HPP
#define IROHA_SHARED_PROTO_MODEL_ENGINE_RECEIPT_HPP

#include "interfaces/query_responses/engine_response_record.hpp"

#include "interfaces/common_objects/types.hpp"
#include "qry_responses.pb.h"

namespace shared_model {
  namespace proto {

    class EngineReceipt final : public interface::EngineReceipt {
     public:
      using TransportType = iroha::protocol::EngineReceipt;

      explicit EngineReceipt(const TransportType &proto);

      explicit EngineReceipt(const EngineReceipt &o);

      shared_model::interface::types::CommandIndexType commandIndex()
          const override;

      /*const shared_model::interface::types::SmartContractCodeType &response()
          const override;*/

     private:
      const TransportType &proto_;
    };
  }  // namespace proto
}  // namespace shared_model

#endif  // IROHA_SHARED_PROTO_MODEL_ENGINE_RECEIPT_HPP
