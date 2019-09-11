/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ametsuchi/newstorage/memory_wsv.hpp"
#include "interfaces/permissions.hpp"
#include <boost/multiprecision/cpp_int.hpp>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <stdexcept>

namespace iroha {
  namespace newstorage {

    using shared_model::interface::RolePermissionSet;
    using shared_model::interface::GrantablePermissionSet;
    using GrantablePermission = shared_model::interface::permissions::Grantable;
    using RolePermission = shared_model::interface::permissions::Role;
    using ID = std::string;
    using AccountID = ID;
    using RoleID = ID;
    using DomainID = ID;
    using AssetID = ID;
    using PK = std::string;
    using NetworkAddress = std::string;
    using uint256_t = boost::multiprecision::uint256_t;

    enum class ResultCode {
      kOk,
      kAccountDoesntExist,
      kPermitteeAccountDoesntExist,
      kRoleDoesntExist,
      kDefaultRoleDoesntExist,
      kDomainDoesntExist,
      kDomainAlreadyExists,
      kAssetAlreadyExists,
      kRoleAlreadyExists,
      kSignatoryAlreadyExists,
      kAccountAlreadyExists,
      kAccountHasNoRole,
      kNoAccountAsset,
      kNotEnoughBalance,
      kAmountOverflow,
      kPeerAlreadyExists,
      kNoPermission,
      kNoCreatorAccount
    };

    template<typename Object> class IdTable {
     public:
      ~IdTable() {
        for (auto& kv: table_) {
          delete kv->second;
        }
      }

      bool whole_table_loaded() const { return loaded_; }
      void whole_table_loaded(bool f) { loaded_ = f; }

      const Object* get(const ID& id) const {
        Object* obj = nullptr;
        auto it = table_.find(id);
        if (it != table_.end()) {
          obj = it->second;
        }
        return obj;
      }

      Object* get(const ID& id, bool* maybe_not_loaded) {
        Object* obj = nullptr;
        auto it = table_.find(id);
        if (it != table_.end()) {
          obj = it->second;
        } else if (maybe_not_loaded) {
          *maybe_not_loaded = not loaded_;
        }
        return obj;
      }

      void put(const ID& id, Object* obj) {
        table_[id] = obj;
      }

     private:
      std::unordered_map<ID, Object*> table_;
      bool loaded_ = false;
    };

    class Roles {
     public:
      // return error if role already exists
      ResultCode append(RoleID id, RolePermissionSet permissions) {
        if (permissions_.count(id) != 0) {
          return ResultCode::kRoleAlreadyExists;
        }
        permissions_[id] = permissions;
        all_roles_.push_back(std::move(id));
        return ResultCode::kOk;
      }

      // returns nulptr if no such a role
      const RolePermissionSet* getRolePermissions(const RoleID& id) const {
        const RolePermissionSet* obj = nullptr;
        auto it = permissions_.find(id);
        if (it != permissions_.end()) {
          obj = &it->second;
        }
        return obj;
      }

     private:
      std::vector<RoleID> all_roles_;
      std::unordered_map<RoleID, RolePermissionSet> permissions_;
    };

    class GrantablePermissions {
     public:
      void append(AccountID from, AccountID to, GrantablePermissionSet permissions) {
        // TODO
      }

      bool has(const AccountID& from, const AccountID& to, GrantablePermission perm) const {
        auto it1 = table_.find(from);
        if (it1 == table_.end()) {
          return false; //load...
        }
        auto it2 = it1->second.find(to);
        if (it2 == it1->second.end()) {
          return false; //load...
        }
        return it2->second.isSet(perm);
      }

      // grant

      // revoke

      // get string

     private:
      using GrantedTo = std::unordered_map<AccountID, GrantablePermissionSet>;
      using GrantedFrom = std::unordered_map<AccountID, GrantedTo>;
      GrantedFrom table_;
    };

    class Domains {
     public:
      // returns false if domain already exists. Check role existence before
      ResultCode append(DomainID id, RoleID default_role_id) {
        if (table_.count(id)) {
          return ResultCode::kDomainAlreadyExists;
        }
        table_.emplace(std::make_pair<DomainID,RoleID>(std::move(id), std::move(default_role_id)));
        return ResultCode::kOk;
      }

      // returns nullptr if no such a domain
      const RoleID* getDefaultRole(const DomainID& id) {
        const ID* role_id = nullptr;
        const auto it = table_.find(id);
        if (it != table_.end()) {
          role_id = &it->second;
        }
        return role_id;
      }

     private:
      // domain id -> default role id
      std::unordered_map<DomainID, RoleID> table_;
    };

    class Signatories {
     public:
      bool hasSignatory(const PK& pk) const {
        return table_.count(pk) > 0;
      }

      void append(const PK& pk) {
        auto it = table_.find(pk);
        if (it == table_.end()) {
          table_[pk] = 1;
        } else {
          ++table_[pk];
        }
      }

      void remove(const PK& pk) {
        auto it = table_.find(pk);
        if (it != table_.end()) {
          if (--(it->second) == 0) {
            table_.erase(it);
          }
        }
      }

     private:
      std::unordered_map<PK, size_t> table_;
    };

    struct Account {
      AccountID id;
      DomainID domain_id;
      std::unordered_set<AssetID> assets;
      std::unordered_set<RoleID> roles;
      std::unordered_set<PK> signatories;
      RolePermissionSet permissions; // recalc if role added
      uint16_t quorum = 0;

      void appendPermissions(const RoleID& role_id, const Roles& role_set) {
        const RolePermissionSet* rp = role_set.getRolePermissions(role_id);
        if (!rp) {
          throw std::runtime_error("MemoryWsv: inconsistent state, role not found");
        }
        permissions |= *rp;
      }

      bool hasPermission(RolePermission perm) const {
        return permissions.isSet(perm);
      }

      bool hasSignatory(const PK& pk) const {
        return signatories.count(pk) > 0;
      }

      bool hasAsset(const AssetID& asset_id) const {
        return assets.count(asset_id) > 0;
      }

      // returns false if role is already there
      ResultCode appendRole(const RoleID& role_id, const Roles& role_set) {
        if (roles.count(id) > 0) {
          return ResultCode::kRoleAlreadyExists;
        }
        appendPermissions(role_id, role_set);
        roles.insert(role_id);
        return ResultCode::kOk;
      }

      // returns false if account has no such a role
      ResultCode detachRole(const RoleID& role_id, const Roles& role_set) {
        if (roles.count(id) != 1) {
          return ResultCode::kAccountHasNoRole;
        }
        roles.erase(role_id);
        permissions.unsetAll();
        for (const auto& r : roles) {
          appendPermissions(r, role_set);
        }
        return ResultCode::kOk;
      }

    };

    struct Asset {
      AssetID id;
      DomainID domain_id;
      uint8_t precision = 0;
      std::unordered_map<AccountID, uint256_t> balances;

      // appends to cache
      void insertAccount(AccountID account, uint256_t amount=uint256_t(0)) {
        balances.emplace(std::move(account), std::move(amount));
      }

      // returns nullptr if no record
      uint256_t* getBalance(const AccountID& account) {
        uint256_t* result = nullptr;
        auto it = balances.find(account);
        if (it != balances.end()) {
          result = &it->second;
        }
        return result;
      }

      ResultCode appendAmount(const AccountID& to, const uint256_t& amount) {
        uint256_t* pTo = getBalance(to);
        if (!pTo) {
          return ResultCode::kNoAccountAsset;
        }
        uint256_t tmp(*pTo);
        tmp += amount;
        if (tmp < *pTo) {
          return ResultCode::kAmountOverflow;
        }
        *pTo = tmp;
        return ResultCode::kOk;
      }

      ResultCode transferAmount(const AccountID& from, const AccountID& to, const uint256_t& amount) {
        uint256_t* pFrom = getBalance(from);
        if (!pFrom) {
          return ResultCode::kNoAccountAsset;
        }
        if (*pFrom < amount) {
          return ResultCode::kNotEnoughBalance;
        }
        ResultCode result = appendAmount(to, amount);
        if (result == ResultCode::kOk) {
          *pFrom -= amount;
        }
        return result;
      }
    };

    class Peers {
     public:
      //returns nullptr if no such a peer
      const NetworkAddress* get(const PK& key) const {
        const NetworkAddress* address = nullptr;
        auto it = table_.find(key);
        if (it != table_.end()) {
          address = &it->second;
        }
        return address;
      }

      ResultCode add(const PK& key, const NetworkAddress& address) {
        if (addresses_.count(address) || table_.count(key) > 0) {
          return ResultCode::kPeerAlreadyExists;
        }
        table_[key] = address;
        addresses_.insert(address);
        return ResultCode::kOk;
      }

      void clear() {
        table_.clear();
        addresses_.clear();
      }
     private:
      std::unordered_map<PK, NetworkAddress> table_;
      std::unordered_set<NetworkAddress> addresses_;
    };

    class MemoryWsv {
     public:
      // Queries
      ResultCode getAccount(
          const AccountID& creator_id,
          const AccountID& account_id,
          DomainID& domain_id,
          uint16_t& quorum,
          //interface::types::JsonType jsonData,
          std::vector<RoleID>& roles
      ) {
        Account* creator = nullptr;
        Account* account = nullptr;
        ResultCode result = loadAccounts(
            creator_id, account_id,
            RolePermission::kGetAllAccounts,
            RolePermission::kGetMyAccount,
            RolePermission::kGetDomainAccounts,
            creator, account
        );
        if (result != ResultCode::kOk) {
          return result;
        }

        domain_id = account->domain_id;
        quorum = account->quorum;
        roles.clear();
        roles.reserve(account->roles.size());
        for (const auto& r : account->roles) {
          roles.push_back(r);
        }

        return ResultCode::kOk;
      }

      ResultCode getSignatories(
          const AccountID& creator_id,
          const AccountID& account_id,
          std::vector<PK>& signatories
      ) {
        Account* creator = nullptr;
        Account* account = nullptr;
        ResultCode result = loadAccounts(
            creator_id, account_id,
            RolePermission::kGetAllSignatories,
            RolePermission::kGetMySignatories,
            RolePermission::kGetDomainSignatories,
            creator, account
        );
        if (result != ResultCode::kOk) {
          return result;
        }

        signatories.clear();
        signatories.reserve(account->signatories.size());
        for (const auto& s : account->signatories) {
          signatories.push_back(s);
        }

        return ResultCode::kOk;
      }

      ResultCode getAccountAssets(
          const AccountID& creator_id,
          const AccountID& account_id,
          std::vector<PK>& signatories
      ) {
        Account* creator = nullptr;
        Account* account = nullptr;
        ResultCode result = loadAccounts(
            creator_id, account_id,
            RolePermission::kGetAllAccAst,
            RolePermission::kGetMyAccAst,
            RolePermission::kGetDomainAccAst,
            creator, account
        );
        if (result != ResultCode::kOk) {
          return result;
        }

        signatories.clear();
        signatories.reserve(account->signatories.size());
        for (const auto& s : account->signatories) {
          signatories.push_back(s);
        }

        return ResultCode::kOk;
      }

      /*
      ResultCode getAccount(
          const AccountID& creator_id,

      );
       */

      /*

      class GetAccountTransactions;
      class GetAccountAssetTransactions;
      class GetTransactions;
      class GetAccountAssets;
      class GetAccountDetail;
      class GetRoles;
      class GetRolePermissions;
      class GetAssetInfo;
      class GetPendingTransactions;
      class GetPeers;
*/
     private:
      ResultCode loadAccounts(
          const AccountID& creator_id,
          const AccountID& account_id,
          RolePermission allPerm,
          RolePermission myPerm,
          RolePermission domainPerm,
          Account*& creator,
          Account*& account)
      {
        bool maybe_not_loaded=false;
        account = nullptr;
        creator = accounts_.get(creator_id, &maybe_not_loaded); // TODO load
        if (!creator) {
          return ResultCode::kNoCreatorAccount;
        }
        if (creator_id == account_id) {
          account = creator;
        }
        bool hasPerm = creator->hasPermission(allPerm);
        if (account == creator) {
          hasPerm = creator->hasPermission(myPerm);
        } else {
          account = accounts_.get(account_id, &maybe_not_loaded);
        }
        if (!account) {
          return hasPerm ? ResultCode::kAccountDoesntExist : ResultCode::kNoPermission;
        }
        if (!hasPerm) {
          hasPerm = (creator->domain_id == account->domain_id &&
                     creator->hasPermission(domainPerm));
        }
        return hasPerm ? ResultCode::kOk : ResultCode::kNoPermission;
      }

      Roles roles_;
      Domains domains_;
      Signatories signatories_;
      Peers peers_;
      IdTable<Asset> assets_;
      IdTable<Account> accounts_;
    };


  }  // namespace newstorage
}  // namespace iroha