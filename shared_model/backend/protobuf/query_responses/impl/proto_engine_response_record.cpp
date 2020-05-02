/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "backend/protobuf/query_responses/proto_engine_response_record.hpp"

#include "backend/protobuf/query_responses/proto_engine_log.hpp"

using namespace shared_model::proto;

EngineReceipt::EngineReceipt(const TransportType &proto)
    : proto_(proto) 
    , engine_logs_{ boost::copy_range<shared_model::interface::EngineReceipt::EngineLogsPtr>(proto_.logs()
        | boost::adaptors::transformed([](auto const &log_entry) {
            return std::make_unique<shared_model::proto::EngineLog>(log_entry);
    }))} 
    { }

EngineReceipt::EngineReceipt(const EngineReceipt &o)
    : EngineReceipt(o.proto_) {}

shared_model::interface::types::CommandIndexType
EngineReceipt::commandIndex() const {
  return proto_.command_index();
}

shared_model::interface::types::HashType const &EngineReceipt::getTxHash() const {
    return proto_.tx_hash();
}

shared_model::interface::types::TxIndexType EngineReceipt::getTxIndex() const {
    return proto_.tx_index();
}

shared_model::interface::types::HeightType EngineReceipt::getBlockHeight() const {
    return proto_.block_height();
}

shared_model::interface::types::HashType const &EngineReceipt::getBlockHash() const {
    return proto_.block_hash();
}

shared_model::interface::types::AccountIdType EngineReceipt::getFrom() const {
    return proto_.from();
}

shared_model::interface::EngineReceipt::PayloadType EngineReceipt::getPayloadType() const {
    if (nullptr != proto_.mutable_to())
        return shared_model::interface::EngineReceipt::PayloadType::kPayloadTypeTo;
    else if (nullptr != proto_.mutable_contract_address())
        return shared_model::interface::EngineReceipt::PayloadType::kPayloadTypeContractAddress;
    else 
        return shared_model::interface::EngineReceipt::PayloadType::kPayloadTypeUnk;
}

shared_model::interface::types::EvmAddressHexString const &EngineReceipt::getPayload() const {
    if (auto ptr = proto_.mutable_to())
        return *ptr;
    else if (auto ptr = proto_.mutable_contract_address())
        return *ptr;
    else 
        return shared_model::interface::types::EvmAddressHexString();
}

shared_model::interface::EngineReceipt::EngineLogsCollectionType const &EngineReceipt::getEngineLogs() const {
    return engine_logs_;
}


/*const shared_model::interface::types::SmartContractCodeType &
EngineReceipt::response() const {
  return proto_.response();
}*/
