/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ametsuchi/newstorage/wsv_cache.hpp"
#include "ametsuchi/newstorage/wsv_sqlite_db.hpp"

namespace iroha {
  namespace newstorage {

    void Roles::load(WsvSqliteDB &db) {
      db.loadRoles(
          [this](const std::string& role, const std::string& permissions) {
            RolePermissionSet p(permissions);
            if (!append(role, p)) {
              throw std::runtime_error("inconsistency in loadRoles");
            }
          }
      );
    }

    bool Roles::append(RoleID id, RolePermissionSet permissions) {
      if (permissions_.count(id) != 0) {
        return false;
      }
      permissions_[id] = permissions;
      all_roles_.push_back(std::move(id));
      return true;
    }

    const RolePermissionSet* Roles::getRolePermissions(const RoleID& id) const {
      const RolePermissionSet* obj = nullptr;
      auto it = permissions_.find(id);
      if (it != permissions_.end()) {
        obj = &it->second;
      }
      return obj;
    }

    void Roles::getRoles(std::vector<RoleID>& out) const {
      out = all_roles_;
    }

    void Domains::load(WsvSqliteDB& db, const Roles& roles) {
      db.loadDomains(
          [this, &roles](const std::string& domain, const std::string& def_role) {
            if (roles.getRolePermissions(def_role) == nullptr) {
              throw std::runtime_error("loadDomains: role not found");
            }
            if (!append(domain, def_role)) {
              throw std::runtime_error("inconsistency in loadDomains");
            }
          }
      );
    }

    bool Domains::append(DomainID id, RoleID default_role_id) {
      if (table_.count(id)) {
        return false;
      }
      table_.emplace(std::make_pair<DomainID,RoleID>(std::move(id), std::move(default_role_id)));
      return true;
    }

    const RoleID* Domains::getDefaultRole(const DomainID& id) const {
      const ID* role_id = nullptr;
      const auto it = table_.find(id);
      if (it != table_.end()) {
        role_id = &it->second;
      }
      return role_id;
    }

    void Peers::load(WsvSqliteDB& db) {
      db.loadPeers(
          [this](const std::string& pk, const std::string& address) {
            if (!append(pk, address))
              throw std::runtime_error("inconsistency in loadPeers");
          }
      );
    }

    bool Peers::append(const PK& key, const NetworkAddress& address) {
      if (addresses_.count(address) || table_.count(key) > 0) {
        return false;
      }
      table_[key] = address;
      addresses_.insert(address);
      return true;
    }

    const NetworkAddress* Peers::get(const PK& key) const {
      const NetworkAddress* address = nullptr;
      auto it = table_.find(key);
      if (it != table_.end()) {
        address = &it->second;
      }
      return address;
    }

    void Peers::get(std::vector<Peer>& peers) const {
      peers.clear();
      peers.reserve(table_.size());
      for (const auto& kv : table_) {
        peers.emplace_back(kv.second, kv.first);
      }
    }

    GrantablePermissions::GrantablePermissions(WsvSqliteDB &db) :
      db_(db)
    {}

    void GrantablePermissions::makeKey(
        const AccountID& from, const AccountID& to
    ) {
      buffer_.clear();
      buffer_.append(from);
      buffer_.push_back(0x01);
      buffer_.append(to);
    }

    bool GrantablePermissions::hasPermission(
        const AccountID& from, const AccountID& to,
        GrantablePermission perm)
    {
      makeKey(from, to);
      auto it = cache_.find(buffer_);
      if (it != cache_.end()) {
        return it->second.isSet(perm);
      }
      std::string perm_string;
      if (db_.loadGrantablePermissions(from, to, perm_string)) {
        GrantablePermissionSet p(perm_string);
        cache_[buffer_] = p;
        return p.isSet(perm);
      }
      cache_[buffer_] = GrantablePermissionSet();
      return false;
    }

    void Assets::load(WsvSqliteDB &db, const Domains& domains) {
      db.loadAssets(
          [this, &domains](
              const std::string& id, const std::string& domain, uint8_t precision
          ) {
            if (domains.getDefaultRole(domain) == nullptr) {
              throw std::runtime_error("loadAssets: domain not found");
            }
            if (getAssetInfo(id) != nullptr) {
              throw std::runtime_error("inconsistency in loadAssets");
            }
            table_[id] = { domain, precision };
          }
      );
    }

    const Assets::Info* Assets::getAssetInfo(const AssetID& id) {
      auto it = table_.find(id);
      if (it == table_.end()) {
        return nullptr;
      }
      return &it->second;
    }


/*
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
