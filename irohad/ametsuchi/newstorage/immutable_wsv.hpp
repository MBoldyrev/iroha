/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_IMMUTABLE_WSV_HPP
#define IROHA_IMMUTABLE_WSV_HPP

#include "wsv_cache.hpp"
#include "ametsuchi/newstorage/wsv_sqlite_db.hpp"
#include "ametsuchi/newstorage/account_detail_storage.hpp"

namespace iroha {
  namespace newstorage {

    class ChangeSet;

    class ImmutableWsv {
     public:
      ImmutableWsv(WsvSqliteDB&& db, AccountDetailStorage&& detail_storage);

      ResultCode getAccount(
          const AccountID& query_initiator_id,
          const AccountID& account_id,
          DomainID& domain_id,
          uint16_t& quorum,
          //interface::types::JsonType jsonData, //TODO
          std::vector<RoleID>& roles
      );

      ResultCode getSignatories(
          const AccountID& query_initiator_id,
          const AccountID& account_id,
          std::vector<PK>& signatories
      );

      ResultCode getAccountAssets(
          const AccountID& query_initiator_id,
          const AccountID& account_id,
          std::vector<AccountAsset>& assets
      );

      ResultCode getRoles(
          const AccountID& query_initiator_id,
          std::vector<RoleID>& roles
      );

      ResultCode getRolePermissions(
          const AccountID& query_initiator_id,
          const RoleID& role_id,
          RolePermissionSet& permissions
      );

      ResultCode getAssetInfo(
          const AccountID& query_initiator_id,
          const AssetID& asset_id,
          DomainID& domain_id,
          uint8_t& precision
      );

      ResultCode getPeers(
          const AccountID& query_initiator_id,
          std::vector<Peer>& peers
      );

      ResultCode getAccountDetail(
          const AccountID& query_initiator_id,
          const AccountID& account_id,
          const AccountID& key,
          const AccountID& writer,
          const ID& start_from_key,
          const AccountID& start_from_writer,
          size_t& total_values,
          std::string& value,
          ID& next_key,
          AccountID& next_writer
      );

      ResultCode getAccountTransactions(
          const AccountID& query_initiator_id
      );

      ResultCode getAccountAssetTransactions(
          const AccountID& query_initiator_id
      );

      ResultCode getTransactions(
          const AccountID& query_initiator_id
      );

      ResultCode getPendingTransactions(
          const AccountID& query_initiator_id
      );

      // drops or reloads caches
      void applyChangeSet(const ChangeSet& change_set);

      ResultCode loadAccountPermission(const AccountID& account_id, RolePermissionSet* perm);

      const std::unordered_set<RoleID>& loadAccountRolesAndPermission(
          const AccountID& account_id, RolePermissionSet* perm);

      bool peerExists(const PK& pk);
      bool accountExists(const AccountID& account_id);

     private:
      ResultCode loadAccounts(
          const AccountID& query_initiator_id,
          const AccountID& account_id,
          RolePermission allPerm,
          RolePermission myPerm,
          RolePermission domainPerm,
          Account*& query_initiator,
          Account*& account);

      ResultCode loadAccount(
          const AccountID& query_initiator_id,
          RolePermission perm,
          Account*& query_initiator);

      WsvSqliteDB db_;
      AccountDetailStorage acc_details_;
      Roles roles_;
      Domains domains_;
      Peers peers_;
      Assets assets_;
      Accounts accounts_;
    };
  }
}
#endif  // IROHA_IMMUTABLE_WSV_HPP
