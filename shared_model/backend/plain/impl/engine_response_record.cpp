/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "backend/plain/engine_response_record.hpp"

using namespace shared_model::interface::types;
using namespace shared_model::plain;

EngineReceipt::EngineReceipt(
    shared_model::interface::types::CommandIndexType    cmd_index,
    shared_model::interface::types::HashType            &&tx_hash,
    shared_model::interface::types::TxIndexType         tx_index,
    shared_model::interface::types::HeightType          block_height,
    shared_model::interface::types::HashType            &&block_hash,
    shared_model::interface::types::AccountIdType       &&account_id_type,
    shared_model::interface::EngineReceipt::PayloadType payload_type,
    shared_model::interface::types::EvmAddressHexString &&payload
    )
    : cmd_index_(cmd_index)
    , tx_hash_(std::move(tx_hash))
    , tx_index_(tx_index)
    , block_height_(block_height)
    , block_hash_(std::move(block_hash))
    , account_id_type_(std::move(account_id_type))
    , payload_type_(payload_type)
    , payload_(std::move(payload))
    { }

shared_model::interface::types::CommandIndexType  EngineReceipt::commandIndex() const {
    return cmd_index_;
}

shared_model::interface::types::HashType const &EngineReceipt::getTxHash() const {
    return tx_hash_;
}

shared_model::interface::types::TxIndexType  EngineReceipt::getTxIndex() const {
    return tx_index_;
}

shared_model::interface::types::HeightType  EngineReceipt::getBlockHeight() const {
    return block_height_;
}

shared_model::interface::types::HashType const &EngineReceipt::getBlockHash() const {
    return block_hash_;
}

shared_model::interface::types::AccountIdType  EngineReceipt::getFrom() const {
    return account_id_type_;
}

shared_model::interface::EngineReceipt::PayloadType  EngineReceipt::getPayloadType() const {
    return payload_type_;
}

shared_model::interface::types::EvmAddressHexString const &EngineReceipt::getPayload() const {
    return payload_;
}

shared_model::interface::EngineReceipt::EngineLogsCollectionType const &EngineReceipt::getEngineLogs() const {
    return engine_logs_;
}

shared_model::interface::EngineReceipt::EngineLogsCollectionType &EngineReceipt::getMutableLogs() {
    return engine_logs_;
}
