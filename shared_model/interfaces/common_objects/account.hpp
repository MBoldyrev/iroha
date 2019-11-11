/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_ACCOUNT_HPP
#define IROHA_SHARED_MODEL_ACCOUNT_HPP

#include "cryptography/hash.hpp"
#include "interfaces/common_objects/trivial_proto.hpp"
#include "interfaces/common_objects/types.hpp"
#include "qry_responses.pb.h"

namespace shared_model {

  /**
   * User identity information in the system
   */
  class Account : public TrivialProto<Account, iroha::protocol::Account> {
   public:
    template <typename T>
    Account(T &&account) : TrivialProto(std::forward<T>(account)) {}

    Account(const types::AccountIdType &account_id,
            const types::DomainIdType &domain_id,
            types::QuorumType quorum,
            const types::JsonType &jsonData);

    /**
     * @return Identity of user, for fetching data
     */
    const types::AccountIdType &accountId() const;

    /**
     * @return Identity of domain, for fetching data
     */
    const types::DomainIdType &domainId() const;

    /**
     * @return Minimum quorum of signatures needed for transactions
     */
    types::QuorumType quorum() const;

    /**
     * @return JSON data stored in account
     */
    const types::JsonType &jsonData() const;

    /**
     * Stringify the data.
     * @return the content of account asset.
     */
    std::string toString() const override;

    /**
     * Checks equality of objects inside
     * @param rhs - other wrapped value
     * @return true, if wrapped objects are same
     */
    bool operator==(const Account &rhs) const override;
  };
}  // namespace shared_model
#endif  // IROHA_SHARED_MODEL_ACCOUNT_HPP
