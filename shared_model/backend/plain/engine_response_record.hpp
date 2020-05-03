/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_PLAIN_ENGINE_RECEIPTS_RESPONSE_RECORD_HPP
#define IROHA_SHARED_MODEL_PLAIN_ENGINE_RECEIPTS_RESPONSE_RECORD_HPP

#include "interfaces/query_responses/engine_response_record.hpp"

#include "interfaces/common_objects/types.hpp"

namespace shared_model {
  namespace plain {

    class EngineReceipt final : public interface::EngineReceipt {
     public:
      using EngineLogsCollection = std::vector<std::unique_ptr<shared_model::plain::EngineLog>>;

      EngineReceipt(
          interface::types::CommandIndexType                  cmd_index,
          interface::types::HashType                          &&tx_hash,
          interface::types::TxIndexType                       tx_index,
          interface::types::HeightType                        block_height,
          interface::types::HashType                          &&block_hash,
          interface::types::AccountIdType                     &&account_id_type,
          interface::EngineReceipt::PayloadType               payload_type,
          interface::types::EvmAddressHexString               &&payload
          );

      interface::types::CommandIndexType commandIndex() const override;
      interface::types::HashType const &getTxHash() const override;
      interface::types::TxIndexType getTxIndex() const override;
      interface::types::HeightType getBlockHeight() const override;
      interface::types::HashType const &getBlockHash() const override;
      interface::types::AccountIdType getFrom() const override;
      interface::EngineReceipt::PayloadType getPayloadType() const override;
      interface::types::EvmAddressHexString const &getPayload() const override;
      interface::EngineLogsCollectionType const &getEngineLogs() const override;

      EngineLogsCollection &getMutableLogs();

     private:
      interface::types::CommandIndexType const    cmd_index_;
      interface::types::HashType const            tx_hash_;
      interface::types::TxIndexType const         tx_index_;
      interface::types::HeightType const          block_height_;
      interface::types::HashType const            block_hash_;
      interface::types::AccountIdType const       account_id_type_;
      interface::EngineReceipt::PayloadType const payload_type_;
      interface::types::EvmAddressHexString const payload_;
      EngineLogsCollection                        engine_logs_;
    };
  }  // namespace plain
}  // namespace shared_model

#endif  // IROHA_SHARED_MODEL_PLAIN_ENGINE_RECEIPTS_RESPONSE_RECORD_HPP
