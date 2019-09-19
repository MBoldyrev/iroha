/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ametsuchi/newstorage/wsv_cache.hpp"
#include "ametsuchi/newstorage/wsv_sqlite_db.hpp"

namespace iroha {
  namespace newstorage {

/*
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

      void getRoles(std::vector<RoleID>& out) const {
        out = all_roles_;
      }

      void load(WsvSqliteDB& db) {
        db.loadRoles(
            [this](const std::string& role, const std::string& permissions) {
              RolePermissionSet p(permissions);
              if (append(role, p) != ResultCode::kOk) {
                throw std::runtime_error("incinsistency in loadRoles");
              }
            }
        );
      }

     private:
      std::vector<RoleID> all_roles_;
      std::unordered_map<RoleID, RolePermissionSet> permissions_;
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

      void load(WsvSqliteDB& db, const Roles& roles) {
        db.loadDomains(
            [this, &roles](const std::string& domain, const std::string& def_role) {
              if (roles.getRolePermissions(def_role) == nullptr) {
                throw std::runtime_error("loadDomains: role not found");
              }
              if (append(domain, def_role) != ResultCode::kOk) {
                throw std::runtime_error("inconsistency in loadDomains");
              }
            }
        );
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
        // TODO diff table

        auto it = table_.find(pk);
        if (it != table_.end()) {
          if (--(it->second) == 0) {
            table_.erase(it);
          }
        }
      }

      void load(WsvSqliteDB& db) {
        db.loadSignatories(
            [this](const std::string& signatory, size_t count) {
              if (count == 0)
                throw std::runtime_error("inconsistency in loadSignatories");
              table_[signatory] = count;
            }
        );
      }

     private:
      std::unordered_map<PK, size_t> table_;
    };

    struct Peer {
      NetworkAddress address;
      PK pub_key;
      // TODO certificate

      Peer(NetworkAddress a, PK pk) : address(std::move(a)), pub_key(std::move(pk))
      {}
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

      void get(std::vector<Peer>& peers) const {
        peers.clear();
        peers.reserve(table_.size());
        for (const auto& kv : table_) {
          peers.emplace_back(kv.second, kv.first);
        }
      }

      ResultCode add(const PK& key, const NetworkAddress& address) {
        if (addresses_.count(address) || table_.count(key) > 0) {
          return ResultCode::kPeerAlreadyExists;
        }
        table_[key] = address;
        addresses_.insert(address);
        return ResultCode::kOk;
      }

      void load(WsvSqliteDB& db) {
        db.loadPeers(
            [this](const std::string& pk, const std::string& address) {
              if (add(pk, address) != ResultCode::kOk)
                throw std::runtime_error("inconsistency in getPeers");
            }
        );
      }

      void clear() {
        table_.clear();
        addresses_.clear();
      }
     private:
      std::unordered_map<PK, NetworkAddress> table_;
      std::unordered_set<NetworkAddress> addresses_;
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



    struct AccountAsset {
      AssetID asset_id;
      uint8_t precision = 0;
      uint256_t balance;
    };

    struct Account {
      AccountID id;
      DomainID domain_id;
      std::unordered_map<AssetID, uint256_t> assets;
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

    class Assets {
     public:
      // appends to cache
      void insertAccount(Account& account, uint256_t amount=uint256_t(0)) {
        account.assets.emplace(id, std::move(amount));
      }

      // returns nullptr if no record
      uint256_t* getBalance(Account& account) {
        uint256_t* result = nullptr;
        auto it = account.assets.find(id);
        if (it != account.assets.end()) {
          result = &it->second;
        }
        return result;
      }

      ResultCode appendAmount(Account& to, const uint256_t& amount) {
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

      ResultCode transferAmount(Account& from, Account& to, const uint256_t& amount) {
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

     private:
      struct Info {
        DomainID domain_id;
        uint8_t precision = 0;
      };
      std::unordered_map<AssetID, Info> table_;
    };
*/
  }  // namespace newstorage
}  // namespace iroha
