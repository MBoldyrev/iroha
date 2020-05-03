/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "interfaces/query_responses/engine_response_record.hpp"

#include "cryptography/hash.hpp"

using namespace shared_model::interface;

bool EngineReceipt::operator==(ModelType const &rhs) const {
    if (&rhs == static_cast<ModelType const*>(this)) {
        return true;
    }

    return commandIndex() == rhs.commandIndex() &&
            getTxHash() == rhs.getTxHash() &&
            getTxIndex() == rhs.getTxIndex() &&
            getBlockHeight() == rhs.getBlockHeight() &&
            getBlockHash() == rhs.getBlockHash() &&
            getFrom() == rhs.getFrom() &&
            getPayloadType() == rhs.getPayloadType() &&
            getPayload() == rhs.getPayload() &&
            getEngineLogs() == rhs.getEngineLogs();
}

std::string EngineReceipt::toString() const {
  return detail::PrettyStringBuilder()
      .init("EngineReceipt")
      .appendNamed("command_index", commandIndex())
      .appendNamed("tx_hash", getTxHash())
      .appendNamed("tx_index", getTxIndex())
      .appendNamed("block_height", getBlockHeight())
      .appendNamed("block_hash", getBlockHash())
      .appendNamed("from", getFrom())
      .appendNamed("payload_type", EngineReceipt::payloadTypeToStr(getPayloadType()))
      .appendNamed("payload", getPayload())
      .appendNamed("engine_logs", getEngineLogs())
      .finalize();
}
