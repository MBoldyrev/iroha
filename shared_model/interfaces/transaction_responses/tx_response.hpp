/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_TX_RESPONSE_HPP
#define IROHA_TX_RESPONSE_HPP

#include "interfaces/base/model_primitive.hpp"

#include "endpoint.pb.h"

#include <boost/variant/variant_fwd.hpp>
#include "interfaces/common_objects/types.hpp"

namespace shared_model {

  class StatelessFailedTxResponse;
  class StatelessValidTxResponse;
  class StatefulFailedTxResponse;
  class StatefulValidTxResponse;
  class RejectedTxResponse;
  class CommittedTxResponse;
  class MstExpiredResponse;
  class NotReceivedTxResponse;
  class MstPendingResponse;
  class EnoughSignaturesCollectedResponse;

  /**
   * TransactionResponse is a status of transaction in system
   */
  class TransactionResponse : public ModelPrimitive<TransactionResponse> {
   private:
    /// const reference shortcut type
    template <typename... Value>
    using wrap = boost::variant<const Value &...>;

   protected:
    /**
     * @return priority of this transaction response; transaction response can
     * only be replaced with one with higher priority
     */
    int priority() const noexcept;

   public:
    using TransportType = iroha::protocol::ToriiResponse;

    TransactionResponse(const TransactionResponse &r);
    TransactionResponse(TransactionResponse &&r) noexcept;

    explicit TransactionResponse(const TransportType &ref);
    explicit TransactionResponse(TransportType &&ref);

    /// Type of variant, that handle all concrete tx responses in the system
    using ResponseVariantType = wrap<StatelessFailedTxResponse,
                                     StatelessValidTxResponse,
                                     StatefulFailedTxResponse,
                                     StatefulValidTxResponse,
                                     RejectedTxResponse,
                                     CommittedTxResponse,
                                     MstExpiredResponse,
                                     NotReceivedTxResponse,
                                     MstPendingResponse,
                                     EnoughSignaturesCollectedResponse>;

    /// Type of transaction hash
    using TransactionHashType = types::HashType;

    /**
     * @return hash of corresponding transaction
     */
    const TransactionHashType &transactionHash() const;

    /**
     * @return attached concrete tx response
     */
    const ResponseVariantType &get() const;

    /// Type of stateless validation error or command name, which failed
    /// validation
    using StatelessErrorOrFailedCommandNameType = std::string;
    /**
     * @return name of the failed command, if present; empty string otherwise
     */
    const StatelessErrorOrFailedCommandNameType &statelessErrorOrCommandName()
        const;

    /// Type of command index, which failed validation
    using FailedCommandIndexType = size_t;
    /**
     * @return index of failed command
     */
    FailedCommandIndexType failedCommandIndex() const;

    /// Type of command error code
    using ErrorCodeType = uint32_t;
    /**
     * @return error code, with which the command failed
     */
    ErrorCodeType errorCode() const;

    /**
     * Enumeration for holding result of priorities comparison
     */
    enum class PrioritiesComparisonResult { kLess, kEqual, kGreater };
    /**
     * Compare priorities of two transaction responses
     * @param other response
     * @return enumeration result of that comparison
     */
    PrioritiesComparisonResult comparePriorities(const ModelType &other) const
        noexcept;

    // ------------------------| Primitive override |-------------------------

    std::string toString() const override;

    bool operator==(const ModelType &rhs) const override;

   private:
    struct Impl;
    std::unique_ptr<Impl> impl_;

    int priority() const noexcept override;
  };
}  // namespace shared_model
#endif  // IROHA_TX_RESPONSE_HPP
