/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_INTERFACE_ENGINE_RESPONSE_RECORD_HPP
#define IROHA_SHARED_MODEL_INTERFACE_ENGINE_RESPONSE_RECORD_HPP

#include <boost/optional.hpp>
#include "interfaces/base/model_primitive.hpp"
#include "interfaces/common_objects/types.hpp"

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
      types::HashType getTxHash() const = 0;

      /// Transaction index
      types::TxIndexType getTxIndex() const = 0;

      /// Block height
      types::HeightType getBlockHeight() const = 0;

      /// Block hash
      types::HashType getBlockHash() const = 0;

      /// Get sender account id
      types::AccountIdType getFrom() const = 0;

      /// Returns the payload data type.
      PayloadType getPayloadType() const = 0;

      /// Returns payload data
      EvmAddressHexString getPayload() const = 0;

      /// Return engine logs collection.
      EngineLogsCollectionType const &getEngineLogs() const = 0;

      std::string toString() const override;

      bool operator==(const ModelType &rhs) const override;
    };

  }  // namespace interface
}  // namespace shared_model

#endif  // IROHA_SHARED_MODEL_INTERFACE_ENGINE_RESPONSE_RECORD_HPP
