/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ametsuchi/newstorage/mutable_wsv.hpp"
#include <stdexcept>

namespace iroha {
  namespace newstorage {
    class ChangeSet {
     public:
      bool peers_changed = false;
      bool signatories_changed = false;
      std::set<AccountID> account_signatories_changed;
      std::set<AccountID> account_roles_changed;
      bool roles_changed = false;

      bool accountChanged(const AccountID& id) {
        return (account_roles_changed.count(id) > 0) ||
            (account_signatories_changed.count(id) > 0);
      }
    };

    ResultCode MutableWsv::checkAccountPermission(
        const AccountID &creator_id,
        RolePermission perm) {
      RolePermissionSet p;
      ResultCode res = getAccountPermissions(creator_id, &p);
      if (res == ResultCode::kOk) {
        if (!p.isSet(perm)) res = ResultCode::kNoPermission;
      }
      return res;
    }

    ResultCode MutableWsv::checkGrantablePermission(
        const AccountID& creator_id, const AccountID& account_id,
        GrantablePermission perm) {
      bool account_exists = change_set_->accountChanged(account_id) ||
          imm_wsv_.accountExists(account_id);
      if (!account_exists) {
        return ResultCode::kAccountDoesntExist;
      }
      std::string str;
      db_.loadGrantablePermissions(account_id, creator_id, str);
      if (not GrantablePermissionSet(str).isSet(perm)) {
        return ResultCode::kNoPermission;
      }
      return ResultCode::kOk;
    }

    ResultCode MutableWsv::getAccountPermissions(
        const AccountID& account_id, RolePermissionSet* perm_set) {
      if (change_set_->account_roles_changed.count(account_id)) {
        std::string str;
        db_.loadAccountPermissions(account_id, str);
        if (str.empty()) {
          return ResultCode::kAccountDoesntExist;
        }
        if (perm_set) {
          *perm_set = RolePermissionSet(str);
        }
        return ResultCode::kOk;
      }
      return imm_wsv_.loadAccountPermission(account_id, perm_set);
    }

    bool MutableWsv::peerExists(const PK &pk) {
      if (change_set_->peers_changed) {
        return db_.peerExists(pk);
      }
      return imm_wsv_.peerExists(pk);
    }

    ResultCode MutableWsv::addPeer(const AccountID &creator_id,
                                   bool do_validation,
                                   const PK &public_key,
                                   const NetworkAddress &address) {

      if (do_validation) {
        ResultCode res = checkAccountPermission(creator_id, RolePermission::kAddPeer);
        if (res != ResultCode::kOk) {
          return res;
        }
        if (peerExists(public_key)) {
          return ResultCode::kPeerAlreadyExists;
        }
      }
      if (!inside_tx_) beginTx();
      try {
        db_.addPeer(public_key, address);
        change_set_->peers_changed = true;
        change_set_->signatories_changed = true;
      } catch (const std::exception& e) {
        return ResultCode::kPeerAlreadyExists;
      }
      return ResultCode::kOk;
    }

    ResultCode MutableWsv::removePeer(const AccountID &creator_id,
                                      bool do_validation,
                                      const PK &public_key) {
      if (do_validation) {
        ResultCode res = checkAccountPermission(creator_id, RolePermission::kRemovePeer);
        if (res != ResultCode::kOk) {
          return res;
        }
        if (!peerExists(public_key)) {
          return ResultCode::kPeerDoesntExist;
        }
      }
      if (!inside_tx_) beginTx();
      try {
  //      db_.decSignatoryRef(public_key);
        db_.removePeer(public_key);
        change_set_->peers_changed = true;
        change_set_->signatories_changed = true;
      } catch (const std::exception& e) {
        return ResultCode::kPeerDoesntExist;
      }
      return ResultCode::kOk;
    }

    ResultCode MutableWsv::addSignatory(const AccountID& creator_id,
                                        const AccountID& account_id,
                                        bool do_validation,
                                        const PK &public_key) {
      if (do_validation) {
        bool has_perm = false;
        bool my_account = (creator_id == account_id);
        RolePermissionSet p;
        ResultCode res = getAccountPermissions(creator_id, &p);
        if (res != ResultCode::kOk) {
          return res;
        }
        if (p.isSet(RolePermission::kAddSignatory) or
          (my_account and p.isSet(RolePermission::kAddMySignatory)))
        {
          has_perm = true;
        }

        if (!has_perm and !my_account) {
          res = checkGrantablePermission(
              creator_id, account_id, GrantablePermission::kAddMySignatory);
        }
        if (res != ResultCode::kOk) {
          return res;
        }
      }
      if (!inside_tx_) beginTx();
      try {
//        db_.incSignatoryRef(public_key);
        db_.addAccountSignatory(account_id, public_key);
        change_set_->account_signatories_changed.insert(account_id);
        change_set_->signatories_changed = true;
      } catch (const std::exception& e) {
        return ResultCode::kSignatoryAlreadyExists;
      }
      return ResultCode::kOk;
    }

    ResultCode MutableWsv::removeSignatory(const AccountID &creator_id,
                                           const AccountID& account_id,
                                           bool do_validation,
                                           const PK &public_key) {
      if (do_validation) {
        bool has_perm = false;
        bool my_account = (creator_id == account_id);
        RolePermissionSet p;
        ResultCode res = getAccountPermissions(creator_id, &p);
        if (res != ResultCode::kOk) {
          return res;
        }
        if (p.isSet(RolePermission::kRemoveSignatory) or
            (my_account and p.isSet(RolePermission::kRemoveMySignatory)))
        {
          has_perm = true;
        }

        if (!has_perm and !my_account) {
          res = checkGrantablePermission(
              creator_id, account_id, GrantablePermission::kRemoveMySignatory);
        }
        if (res != ResultCode::kOk) {
          return res;
        }
      }
      if (!inside_tx_) beginTx();
      try {
//        db_.decSignatoryRef(public_key);
 //       db_.removeAccountSignatory(account_id, public_key);
        change_set_->account_signatories_changed.insert(account_id);
        change_set_->signatories_changed = true;
      } catch (const std::exception& e) {
        return ResultCode::kDataIntegrityError;
      }
      return ResultCode::kOk;
    }

    ResultCode MutableWsv::appendRole(const AccountID &creator_id,
                                      bool do_validation,
                                      const AccountID &account_id,
                                      const RoleID &role_id) {
      if (do_validation) {
        ResultCode res = checkAccountPermission(creator_id, RolePermission::kAppendRole);
        if (res != ResultCode::kOk) {
          return res;
        }
      }
      if (!inside_tx_) beginTx();
      try {
        RolePermissionSet account_perms, role_perms;
        if (getAccountPermissions(account_id, &account_perms) != ResultCode::kOk) {
          return ResultCode::kAccountDoesntExist;
        }
        if (getRolePermissions(role_id, &role_perms) != ResultCode::kOk) {
          return ResultCode::kRoleDoesntExist;
        }
        db_.attachAccountRole(account_id, role_id);
        RolePermissionSet new_perms(account_perms);
        new_perms |= role_perms;
        if (new_perms != account_perms) {
          db_.updateAccountPermissions(account_id, new_perms.toBitstring());
        }
        change_set_->account_roles_changed.insert(account_id);
      } catch (const std::exception& e) {
        return ResultCode::kDataIntegrityError;
      }
      return ResultCode::kOk;
    }

    /*
    ResultCode MutableWsv::detachRole(const AccountID &creator_id,
                                      bool do_validation,
                                      const AccountID &account_id,
                                      const RoleID &role_id) {
      if (do_validation) {
        ResultCode res = checkAccountPermission(creator_id, RolePermission::kDetachRole);
        if (res != ResultCode::kOk) {
          return res;
        }
      }
      if (!inside_tx_) beginTx();
      try {
        //RolePermissionSet account_perms, role_perms;
        //if (getAccountPermissions(account_id, &account_perms) != ResultCode::kOk) {
        //  return ResultCode::kAccountDoesntExist;
        //}
        //if (getRolePermissions(role_id, &role_perms) != ResultCode::kOk) {
        //  return ResultCode::kRoleDoesntExist;
        //}
        db_.detachAccountRole(account_id, role_id);



        const std::unordered_set<RoleID>& loadAccountRolesAndPermission(
            const AccountID& account_id, RolePermissionSet* perm);


        RolePermissionSet new_perms(account_perms);
        new_perms |= role_perms;
        if (new_perms != account_perms) {
          db_.updateAccountPermissions(account_id, new_perms.toBitstring());
        }
        change_set_->account_roles_changed.insert(account_id);
      } catch (const std::exception& e) {
        return ResultCode::kDataIntegrityError;
      }
      return ResultCode::kOk;
    }


    ResultCode MutableWsv::createAccount(const AccountID &creator_id,
                                         bool do_validation,
                                         const AccountID &account_id,
                                         const DomainID &domain_id,
                                         uint16_t quorum) {
      if (do_validation) {
        ResultCode res = checkAccountPermission(creator_id, RolePermission::kCreateAccount);
        if (res != ResultCode::kOk) {
          return res;
        }
      }
      if (!inside_tx_) beginTx();
      try {


        RolePermissionSet account_perms, role_perms;
        if (getAccountPermissions(account_id, &account_perms) != ResultCode::kOk) {
          return ResultCode::kAccountDoesntExist;
        }
        if (getRolePermissions(role_id, &role_perms) != ResultCode::kOk) {
          return ResultCode::kRoleDoesntExist;
        }
        db_.attachAccountRole(account_id, role_id);
        RolePermissionSet new_perms(account_perms);
        new_perms |= role_perms;
        if (new_perms != account_perms) {
          db_.updateAccountPermissions(account_id, new_perms.toBitstring());
        }
        change_set_->account_roles_changed.insert(account_id);
      } catch (const std::exception& e) {
        return ResultCode::kDataIntegrityError;
      }
      return ResultCode::kOk;
    }






    ResultCode MutableWsv::createAccount(const AccountID &creator_id,
                                         bool do_validation,
                                         const AccountID &account_id,
                                         const DomainID &domain_id,
                                         uint16_t quorum);

    ResultCode MutableWsv::createAsset(const AccountID &creator_id,
                                       bool do_validation,
                                       const AssetID &asset_id,
                                       const DomainID &domain_id,
                                       uint8_t precision);

    ResultCode MutableWsv::createDomain(const AccountID &creator_id,
                                        bool do_validation,
                                        const DomainID &domain_id,
                                        const RoleID &role_id);

    ResultCode MutableWsv::createRole(const AccountID &creator_id,
                                      bool do_validation,
                                      const RoleID &role_id,
                                      const RolePermissionSet &perm);

    ResultCode MutableWsv::grantPermission(const AccountID &creator_id,
                                           bool do_validation,
                                           const AccountID &account_id,
                                           const GrantablePermissionSet &perm);

    ResultCode MutableWsv::revokePermission(const AccountID &creator_id,
                                            bool do_validation,
                                            const AccountID &account_id,
                                            const GrantablePermissionSet &perm);

    ResultCode MutableWsv::setAccountDetail(const AccountID &creator_id,
                                            bool do_validation,
                                            const AccountID &account_id,
                                            const ID &key,
                                            const std::string &value);

    ResultCode MutableWsv::compareAndSetAccountDetail(
        const AccountID &creator_id,
        bool do_validation,
        const AccountID &account_id,
        const ID &key,
        const std::string expected,
        const std::string &value);

    ResultCode MutableWsv::setQuorum(const AccountID &creator_id,
                                     bool do_validation,
                                     const AccountID &account_id,
                                     uint16_t quorum);
*/

    ResultCode MutableWsv::setSettingValue(
        const AccountID &creator_id,
        bool do_validation,
        const shared_model::interface::types::SettingKeyType &key,
        const shared_model::interface::types::SettingValueType &value) {
      if (!inside_tx_) beginTx();
      try {
        db_.setSettingValue(key, value);
      } catch (const std::exception& e) {
        return ResultCode::kDataIntegrityError;
      }
      return ResultCode::kOk;
    }

  }  // namespace newstorage
}  // namespace iroha
