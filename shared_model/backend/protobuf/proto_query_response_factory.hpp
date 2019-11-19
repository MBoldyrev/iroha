/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_PROTO_QUERY_RESPONSE_FACTORY_HPP
#define IROHA_PROTO_QUERY_RESPONSE_FACTORY_HPP

#include "interfaces/iroha_internal/query_response_factory.hpp"

namespace shared_model {

  class ProtoQueryResponseFactory : public QueryResponseFactory {
   public:
    std::unique_ptr<QueryResponse> createAccountAssetResponse(
        std::vector<std::tuple<types::AccountIdType,
                               types::AssetIdType,
                               shared_model::Amount>> assets,
        size_t total_assets_number,
        boost::optional<shared_model::types::AssetIdType> next_asset_id,
        const crypto::Hash &query_hash) const override;

    std::unique_ptr<QueryResponse> createAccountDetailResponse(
        types::DetailType account_detail,
        size_t total_number,
        boost::optional<const shared_model::AccountDetailRecordId &>
            next_record_id,
        const crypto::Hash &query_hash) const override;

    std::unique_ptr<QueryResponse> createAccountResponse(
        types::AccountIdType account_id,
        types::DomainIdType domain_id,
        types::QuorumType quorum,
        types::JsonType jsonData,
        std::vector<std::string> roles,
        const crypto::Hash &query_hash) const override;

    std::unique_ptr<QueryResponse> createBlockResponse(
        std::unique_ptr<Block> block,
        const crypto::Hash &query_hash) const override;

    std::unique_ptr<QueryResponse> createErrorQueryResponse(
        ErrorQueryType error_type,
        ErrorQueryResponse::ErrorMessageType error_msg,
        ErrorQueryResponse::ErrorCodeType error_code,
        const crypto::Hash &query_hash) const override;

    std::unique_ptr<QueryResponse> createSignatoriesResponse(
        std::vector<types::PubkeyType> signatories,
        const crypto::Hash &query_hash) const override;

    std::unique_ptr<QueryResponse> createTransactionsResponse(
        std::vector<std::unique_ptr<shared_model::Transaction>> transactions,
        const crypto::Hash &query_hash) const override;

    std::unique_ptr<QueryResponse> createTransactionsPageResponse(
        std::vector<std::unique_ptr<shared_model::Transaction>> transactions,
        boost::optional<const crypto::Hash &> next_tx_hash,
        types::TransactionsNumberType all_transactions_size,
        const crypto::Hash &query_hash) const override;

    std::unique_ptr<QueryResponse> createPendingTransactionsPageResponse(
        std::vector<std::unique_ptr<shared_model::Transaction>> transactions,
        types::TransactionsNumberType all_transactions_size,
        boost::optional<PendingTransactionsPageResponse::BatchInfo>
            next_batch_info,
        const crypto::Hash &query_hash) const override;

    std::unique_ptr<QueryResponse> createAssetResponse(
        types::AssetIdType asset_id,
        types::DomainIdType domain_id,
        types::PrecisionType precision,
        const crypto::Hash &query_hash) const override;

    std::unique_ptr<QueryResponse> createRolesResponse(
        std::vector<types::RoleIdType> roles,
        const crypto::Hash &query_hash) const override;

    std::unique_ptr<QueryResponse> createRolePermissionsResponse(
        RolePermissionSet role_permissions,
        const crypto::Hash &query_hash) const override;

    std::unique_ptr<QueryResponse> createPeersResponse(
        types::PeerList peers, const crypto::Hash &query_hash) const override;

    std::unique_ptr<BlockQueryResponse> createBlockQueryResponse(
        std::shared_ptr<const Block> block) const override;

    std::unique_ptr<BlockQueryResponse> createBlockQueryResponse(
        std::string error_message) const override;
  };

}  // namespace shared_model

#endif  // IROHA_PROTO_QUERY_RESPONSE_FACTORY_HPP
