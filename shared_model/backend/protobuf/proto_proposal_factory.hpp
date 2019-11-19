/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_PROTO_PROPOSAL_FACTORY_HPP
#define IROHA_PROTO_PROPOSAL_FACTORY_HPP

#include "interfaces/iroha_internal/proposal_factory.hpp"
#include "interfaces/iroha_internal/unsafe_proposal_factory.hpp"
#include "validators/validators_common.hpp"

#include "backend/protobuf/proposal.hpp"
#include "backend/protobuf/transaction.hpp"
#include "proposal.pb.h"

namespace shared_model {
  template <typename Validator>
  class ProtoProposalFactory : public ProposalFactory,
                               public UnsafeProposalFactory {
   public:
    using TransactionsCollectionType =
        ProposalFactory::TransactionsCollectionType;
    using UnsafeTransactionsCollectionType =
        UnsafeProposalFactory::TransactionsCollectionType;

    ProtoProposalFactory(std::shared_ptr<validation::ValidatorsConfig> config)
        : validator_{config} {}

    FactoryResult<std::unique_ptr<Proposal>> createProposal(
        types::HeightType height,
        types::TimestampType created_time,
        TransactionsCollectionType transactions) override {
      return createProposal(
          createProtoProposal(height, created_time, transactions));
    }

    // TODO mboldyrev 13.02.2019 IR-323
    // make it return std::shared_ptr<const Proposal>
    std::unique_ptr<Proposal> unsafeCreateProposal(
        types::HeightType height,
        types::TimestampType created_time,
        UnsafeTransactionsCollectionType transactions) override {
      return std::make_unique<Proposal>(
          createProtoProposal(height, created_time, transactions));
    }

    /**
     * Create and validate proposal using protobuf object
     */
    FactoryResult<std::unique_ptr<Proposal>> createProposal(
        const iroha::protocol::Proposal &proposal) {
      return validate(std::make_unique<Proposal>(proposal));
    }

   private:
    iroha::protocol::Proposal createProtoProposal(
        types::HeightType height,
        types::TimestampType created_time,
        UnsafeTransactionsCollectionType transactions) {
      iroha::protocol::Proposal proposal;

      proposal.set_height(height);
      proposal.set_created_time(created_time);

      for (const auto &tx : transactions) {
        *proposal.add_transactions() =
            static_cast<const shared_model::Transaction &>(tx).getTransport();
      }

      return proposal;
    }

    FactoryResult<std::unique_ptr<Proposal>> validate(
        std::unique_ptr<Proposal> proposal) {
      auto errors = validator_.validate(*proposal);

      if (errors) {
        return iroha::expected::makeError(errors.reason());
      }

      return iroha::expected::makeValue<std::unique_ptr<Proposal>>(
          std::move(proposal));
    }

    Validator validator_;
  };
}  // namespace shared_model

#endif  // IROHA_PROTO_PROPOSAL_FACTORY_HPP
