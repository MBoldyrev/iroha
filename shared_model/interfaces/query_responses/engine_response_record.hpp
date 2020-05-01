/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_INTERFACE_ENGINE_RECEIPT_HPP
#define IROHA_SHARED_MODEL_INTERFACE_ENGINE_RECEIPT_HPP

#include <boost/optional.hpp>
#include "interfaces/base/model_primitive.hpp"
#include "interfaces/common_objects/types.hpp"
#include "interfaces/query_responses/engine_log.hpp"

namespace shared_model {
  namespace interface {

    /// Provides a response for a single corresponding EngineCall command
    class EngineReceipt : public ModelPrimitive<EngineReceipt> {
     public:

      enum struct PayloadType {
        kPayloadTypeTo,
        kPayloadTypeContractAddress
      };

      using EngineLogsCollectionType = std::vector<interface::EngineLog>;

      /// Get the index
      virtual interface::types::CommandIndexType commandIndex() const = 0;

      /// Transaction hash
      virtual types::HashType const &getTxHash() const = 0;

      /// Transaction index
      virtual types::TxIndexType getTxIndex() const = 0;

      /// Block height
      virtual types::HeightType getBlockHeight() const = 0;

      /// Block hash
      virtual types::HashType const &getBlockHash() const = 0;

      /// Get sender account id
      virtual types::AccountIdType getFrom() const = 0;

      /// Returns the payload data type.
      virtual PayloadType getPayloadType() const = 0;

      /// Returns payload data
      virtual EvmAddressHexString const &getPayload() const = 0;

      /// Return engine logs collection.
      virtual EngineLogsCollectionType const &const &getEngineLogs() const = 0;

      std::string toString() const override;

      bool operator==(const ModelType &rhs) const override;
    };

  }  // namespace interface
}  // namespace shared_model

#endif  // IROHA_SHARED_MODEL_INTERFACE_ENGINE_RECEIPT_HPP
