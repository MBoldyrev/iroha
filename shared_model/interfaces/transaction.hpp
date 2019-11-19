/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_TRANSACTION_HPP
#define IROHA_SHARED_MODEL_TRANSACTION_HPP

#include "common/cloneable.hpp"
#include "interfaces/base/signable.hpp"
#include "interfaces/common_objects/types.hpp"

namespace shared_model {

  class BatchMeta;
  class Command;

  /**
   * Transaction class represent well-formed intent from client to change
   * state of ledger.
   */
  class Transaction : public Signable<Transaction>,
                      public Cloneable<Transaction> {
   public:
    /**
     * @return creator of transaction
     */
    const types::AccountIdType &creatorAccountId() const;

    /**
     * @return quorum of transaction
     */
    types::QuorumType quorum() const;

    /// Type of ordered collection of commands
    using CommandsType = boost::
        any_range<Command, boost::random_access_traversal_tag, const Command &>;

    /**
     * @return attached commands
     */
    CommandsType commands() const;

    /**
     * @return object payload (everything except signatures)
     */
    const types::BlobType &reducedPayload() const;

    // TODO [IR-1874] Akvinikym 16.11.18: rename the field
    /**
     * @return hash of reduced payload
     */
    const types::HashType &reducedHash() const;

    /*
     * @return Batch Meta if exists
     */
    boost::optional<std::shared_ptr<BatchMeta>> batchMeta() const;

    std::string toString() const override;
  };

}  // namespace shared_model
#endif  // IROHA_SHARED_MODEL_TRANSACTION_HPP
