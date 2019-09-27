/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_MUTABLE_WSV_HPP
#define IROHA_MUTABLE_WSV_HPP

#include "ametsuchi/newstorage/immutable_wsv.hpp"

namespace iroha {
  namespace newstorage {

    class MutableWsv {
     public:
      MutableWsv(WsvSqliteDB&& db, ImmutableWsv& imm_wsv);

      void beginTx();

      // ... and apply changeset
      void commitTx();
      void rollbackTx();
      void createSavepoint();
      void releaseToSavepoint();
      void rollbackToSavepoint();

      ResultCode addPeer(const AccountID& creator_id, bool do_validation,
          const PK &public_key, const NetworkAddress &address);

      ResultCode removePeer(const AccountID& creator_id, bool do_validation,
          const PK &public_key);

      ResultCode addSignatory(const AccountID& creator_id,
          const AccountID& account_id,
          bool do_validation,
          const PK &public_key);

      ResultCode removeSignatory(const AccountID& creator_id,
          const AccountID& account_id,
          bool do_validation,
          const PK &public_key);

      ResultCode appendRole(const AccountID& creator_id, bool do_validation,
          const AccountID& account_id, const RoleID& role_id);

      ResultCode detachRole(const AccountID& creator_id, bool do_validation,
          const AccountID& account_id, const RoleID& role_id);

      ResultCode createAccount(const AccountID& creator_id, bool do_validation,
          const AccountID &account_id, const DomainID &domain_id,
          uint16_t quorum);

      ResultCode createAsset(const AccountID& creator_id, bool do_validation,
          const AssetID &asset_id,
          const DomainID& domain_id, uint8_t precision);

      ResultCode createDomain(const AccountID& creator_id, bool do_validation,
          const DomainID &domain_id, const RoleID &role_id);

      ResultCode createRole(const AccountID& creator_id, bool do_validation,
          const RoleID &role_id, const RolePermissionSet& perm);

      ResultCode grantPermission(const AccountID& creator_id, bool do_validation,
          const AccountID &account_id, const GrantablePermissionSet& perm);

      ResultCode revokePermission(const AccountID& creator_id, bool do_validation,
          const AccountID &account_id, const GrantablePermissionSet& perm);

      ResultCode setAccountDetail(const AccountID& creator_id, bool do_validation,
          const AccountID &account_id, const ID& key, const std::string& value);

      ResultCode compareAndSetAccountDetail(const AccountID& creator_id, bool do_validation,
          const AccountID &account_id, const ID& key,
          const std::string expected, const std::string& value);

      ResultCode setQuorum(const AccountID& creator_id, bool do_validation,
          const AccountID &account_id, uint16_t quorum);

      //ResultCode addAssetQuantity(const AccountID& creator_id, bool do_validation, );
      //ResultCode subtractAssetQuantity(const AccountID& creator_id, bool do_validation, );
      //ResultCode transferAsset(const AccountID& creator_id, bool do_validation, );


     private:
      // checks account existence and permission
      ResultCode checkAccountPermission(
          const AccountID& creator_id, RolePermission perm);

      ResultCode checkGrantablePermission(
          const AccountID& creator_id, const AccountID& account_id, GrantablePermission perm);

      ResultCode getAccountPermissions(
          const AccountID& account_id, RolePermissionSet* perm_set);

      ResultCode getRolePermissions(
          const RoleID& role_id, RolePermissionSet* perm_set);

      bool peerExists(const PK& pk);
      bool roleExists(const RoleID& role);

      WsvSqliteDB db_;
      ImmutableWsv& imm_wsv_;
      std::unique_ptr<ChangeSet> change_set_;
      std::string savepoint_;
      bool inside_tx_ = false;
    };
  }
}
#endif  // IROHA_MUTABLE_WSV_HPP
