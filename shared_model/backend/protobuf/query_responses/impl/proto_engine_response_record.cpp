/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "backend/protobuf/query_responses/proto_engine_response_record.hpp"

using namespace shared_model::proto;

EngineReceipt::EngineReceipt(const TransportType &proto)
    : proto_(proto) {}

EngineReceipt::EngineReceipt(const EngineReceipt &o)
    : EngineReceipt(o.proto_) {}

shared_model::interface::types::CommandIndexType
EngineReceipt::commandIndex() const {
  return proto_.command_index();
}

/*const shared_model::interface::types::SmartContractCodeType &
EngineReceipt::response() const {
  return proto_.response();
}*/
