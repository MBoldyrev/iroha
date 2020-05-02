/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "backend/plain/engine_response_record.hpp"

using namespace shared_model::interface::types;
using namespace shared_model::plain;

EngineReceipt::EngineReceipt(
    interface::types::CommandIndexType                  cmd_index,
    interface::types::HashType                          &&tx_hash,
    interface::types::TxIndexType                       tx_index,
    interface::types::HeightType                        block_height,
    interface::types::HashType                          &&block_hash,
    interface::types::AccountIdType                     &&account_id_type,
    interface::EngineReceipt::PayloadType               payload_type,
    interface::types::EvmAddressHexString               &&payload,
    interface::EngineReceipt::EngineLogsCollectionType  &&engine_logs
    ) 
    : cmd_index_(cmd_index)
    , tx_hash_(std::move(tx_hash))
    , tx_index_(tx_index)
    , block_height_(block_height)
    , block_hash_(std::move(block_hash))
    , account_id_type_(std::move(account_id_type))
    , payload_type_(payload_type)
    , payload_(std::move(payload))
    , engine_logs_(std::move(engine_logs))
    { }

interface::types::CommandIndexType  EngineReceipt::commandIndex() const {
    return cmd_index_;
}

interface::types::HashType const &EngineReceipt::getTxHash() const {
    return tx_hash_;
}

interface::types::TxIndexType  EngineReceipt::getTxIndex() const {
    return tx_index_;
}

interface::types::HeightType  EngineReceipt::getBlockHeight() const {
    return block_height_;
}

interface::types::HashType const &EngineReceipt::getBlockHash() const {
    return block_hash_;
}

interface::types::AccountIdType  EngineReceipt::getFrom() const {
    return account_id_type_;
}

interface::EngineReceipt::PayloadType  EngineReceipt::getPayloadType() const {
    return payload_type_;
}

interface::types::EvmAddressHexString const &EngineReceipt::getPayload() const {
    return payload_;
}

interface::EngineLogsCollectionType const &EngineReceipt::getEngineLogs() const {
    return engine_logs_;
}
