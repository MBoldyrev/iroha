/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "backend/protobuf/query_responses/proto_engine_log.hpp"

using namespace shared_model::proto;

EngineLog::EngineLog(const TransportType &proto)
    : proto_(proto) {}

EngineLog::EngineLog(const EngineLog &o)
    : EngineLog(o.proto_) {}

shared_model::interface::types::EvmAddressHexString const &
getAddress() const {
    return proto_.address();
}

shared_model::interface::types::EvmDataHexString const &
getData() const {
    return proto_.data();
}

shared_model::interface::EngineLog::TopicsCollectionType const &
getTopics() const {
    return proto_.topics();
}
