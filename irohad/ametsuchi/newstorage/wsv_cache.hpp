/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_WSV_CACHE_HPP
#define IROHA_WSV_CACHE_HPP

#include "result_code.hpp"
#include "interfaces/permissions.hpp"
#include <boost/multiprecision/cpp_int.hpp>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>

namespace iroha {
  namespace newstorage {

    class WsvSqliteDB;

    using ID = std::string;
    using AccountID = ID;
    using shared_model::interface::RolePermissionSet;
    using shared_model::interface::GrantablePermissionSet;
    using GrantablePermission = shared_model::interface::permissions::Grantable;
    using RolePermission = shared_model::interface::permissions::Role;
    using RoleID = ID;
    using DomainID = ID;
    using AssetID = ID;
    using PK = std::string;
    using NetworkAddress = std::string;
    using uint256_t = boost::multiprecision::uint256_t;

    class Roles {
     public:
      // throws on error
      void load(WsvSqliteDB& db);

      // returns nullptr if no such a role, permissions otherwise
      const RolePermissionSet* getRolePermissions(const RoleID& id) const;

      // returns all roles
      void getRoles(std::vector<RoleID>& out) const;

     private:
      // returns false if role already exists
      bool append(RoleID id, RolePermissionSet permissions);

      std::vector<RoleID> all_roles_;
      std::unordered_map<RoleID, RolePermissionSet> permissions_;
    };

    class Domains {
     public:
      void load(WsvSqliteDB& db, const Roles& roles);

      // returns nullptr if no such a domain
      const RoleID* getDefaultRole(const DomainID& id) const;

     private:
      // returns false if domain already exists. Check role existence before
      bool append(DomainID id, RoleID default_role_id);

      // domain id -> default role id
      std::unordered_map<DomainID, RoleID> table_;
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
      void load(WsvSqliteDB& db);

      //returns nullptr if no such a peer
      const NetworkAddress* get(const PK& key) const;

      // returns all peers
      void get(const std::function<void(const std::string&, const std::string&)>& callback) const;

     private:
      // returns false if peer already exists (ether PK or address)
      bool append(const PK& key, const NetworkAddress& address);

      std::unordered_map<PK, NetworkAddress> table_;
      std::unordered_set<NetworkAddress> addresses_;
    };


    class GrantablePermissions {
     public:
      explicit GrantablePermissions(WsvSqliteDB& db);

      bool hasPermission(const AccountID& from, const AccountID& to, GrantablePermission perm);

     private:
      void makeKey(const AccountID& from, const AccountID& to);

      WsvSqliteDB& db_;
      std::unordered_map<ID, GrantablePermissionSet> cache_;
      std::string buffer_;
    };

    class Assets {
     public:
      void load(WsvSqliteDB& db, const Domains& domains);

      struct Info {
        DomainID domain_id;
        uint8_t precision = 0;
      };

      // returns nullptr if no asset
      const Info* getAssetInfo(const AssetID& id);

     private:

      std::unordered_map<AssetID, Info> table_;
    };

    template<typename Object> class IdTable {
     public:
      ~IdTable() {
        for (auto& kv: table_) {
          if (kv.second) delete kv.second;
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

    struct AccountAsset {
      AssetID asset_id;
      uint8_t precision = 0;
      uint256_t balance;
    };

    struct Account {
      AccountID id;
      DomainID domain_id;

      std::unordered_set<RoleID> roles;
      std::unordered_set<PK> signatories;
      std::unordered_map<AssetID, AccountAsset> assets;
      RolePermissionSet permissions; // recalc if role added
      uint16_t quorum = 0;
      bool assets_loaded = false;

      const AccountAsset* getAccountAsset(const AssetID& asset);

      ResultCode getAccountAssets(std::vector<AccountAsset>& assets);

      void appendPermissions(const RoleID& role_id, const Roles& role_set);

      bool hasPermission(RolePermission perm) const;

      bool hasSignatory(const PK& pk) const;

      //bool hasAsset(const AssetID& asset_id) const;

      bool load(WsvSqliteDB& db);

     private:
      // returns false if role is already there
      ResultCode appendRole(const RoleID& role_id, const Roles& role_set);
      // returns false if account has no such a role
      ResultCode detachRole(const RoleID& role_id, const Roles& role_set);

    };

    class Accounts {
     public:
      explicit Accounts(WsvSqliteDB& db);

      // returns nullptr if no account with id
      Account* getAccount(const AccountID& id);

      void loadAccountAssets(Account& account);

     private:
      WsvSqliteDB& db_;
      IdTable<Account> accounts_;
    };

  }  // namespace newstorage
}  // namespace iroha

#endif //IROHA_WSV_CACHE_HPP
