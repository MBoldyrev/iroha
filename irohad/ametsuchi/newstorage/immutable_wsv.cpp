/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ametsuchi/newstorage/immutable_wsv.hpp"
#include <stdexcept>

namespace iroha {
  namespace newstorage {

    ImmutableWsv::ImmutableWsv(
        WsvSqliteDB&& db, AccountDetailStorage&& detail_storage
    ) :
      db_(std::move(db)),
      acc_details_(std::move(detail_storage)),
      accounts_(db_)
    {
      roles_.load(db_);
      domains_.load(db_, roles_);
      peers_.load(db_);
    }

    ResultCode ImmutableWsv::getAccount(
        const AccountID& query_initiator_id,
        const AccountID& account_id,
        DomainID& domain_id,
        uint16_t& quorum,
        //interface::types::JsonType jsonData,
        std::vector<RoleID>& roles
    ) {
      Account* query_initiator = nullptr;
      Account* account = nullptr;
      ResultCode result = loadAccounts(
          query_initiator_id, account_id,
          RolePermission::kGetAllAccounts,
          RolePermission::kGetMyAccount,
          RolePermission::kGetDomainAccounts,
          query_initiator, account
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

    ResultCode ImmutableWsv::getSignatories(
        const AccountID& query_initiator_id,
        const AccountID& account_id,
        std::vector<PK>& signatories
    ) {
      Account* query_initiator = nullptr;
      Account* account = nullptr;
      ResultCode result = loadAccounts(
          query_initiator_id, account_id,
          RolePermission::kGetAllSignatories,
          RolePermission::kGetMySignatories,
          RolePermission::kGetDomainSignatories,
          query_initiator, account
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

    ResultCode ImmutableWsv::getAccountAssets(
        const AccountID& query_initiator_id,
        const AccountID& account_id,
        std::vector<AccountAsset>& assets
    ) {
      Account* query_initiator = nullptr;
      Account* account = nullptr;
      ResultCode result = loadAccounts(
          query_initiator_id, account_id,
          RolePermission::kGetAllAccAst,
          RolePermission::kGetMyAccAst,
          RolePermission::kGetDomainAccAst,
          query_initiator, account
      );
      if (result != ResultCode::kOk) {
        return result;
      }

      return account->getAccountAssets(assets);
    }

    ResultCode ImmutableWsv::getRoles(
        const AccountID& query_initiator_id,
        std::vector<RoleID>& roles
    ) {
      Account* query_initiator = nullptr;
      ResultCode rc = loadAccount(
          query_initiator_id,
          RolePermission::kGetRoles, query_initiator);
      if (rc == ResultCode::kOk) {
        roles_.getRoles(roles);
      }
      return rc;
    }

    ResultCode ImmutableWsv::getRolePermissions(
        const AccountID& query_initiator_id,
        const RoleID& role_id,
        RolePermissionSet& permissions
    ) {
      Account* query_initiator = nullptr;
      ResultCode rc = loadAccount(
          query_initiator_id,
          RolePermission::kGetRoles, query_initiator);
      if (rc == ResultCode::kOk) {
        const RolePermissionSet* p = roles_.getRolePermissions(role_id);
        if (p) {
          permissions = *p;
        } else {
          rc = ResultCode::kRoleDoesntExist;
        }
      }
      return rc;
    }

    ResultCode ImmutableWsv::getAssetInfo(
        const AccountID& query_initiator_id,
        const AssetID& asset_id,
        DomainID& domain_id,
        uint8_t& precision
    ) {
      Account* query_initiator = nullptr;
      ResultCode rc = loadAccount(
          query_initiator_id,
          RolePermission::kReadAssets, query_initiator);
      if (rc == ResultCode::kOk) {
        const Assets::Info* info = assets_.getAssetInfo(asset_id);
        if (info) {
          domain_id = info->domain_id;
          precision = info->precision;
        } else {
          rc = ResultCode::kAssetDoesntExist;
        }
      }
      return rc;
    }

    ResultCode ImmutableWsv::getPeers(
        const AccountID& query_initiator_id,
        std::vector<Peer>& peers
    ) {
      Account* query_initiator = nullptr;
      ResultCode rc = loadAccount(
          query_initiator_id,
          RolePermission::kGetPeers, query_initiator);
      if (rc == ResultCode::kOk) {
        peers_.get(peers);
      }
      return rc;
    }

    ResultCode ImmutableWsv::getAccountDetail(
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
    ) {
      Account* query_initiator = nullptr;
      Account* account = nullptr;
      ResultCode result = loadAccounts(
          query_initiator_id, account_id,
          RolePermission::kGetAllAccDetail,
          RolePermission::kGetMyAccDetail,
          RolePermission::kGetDomainAccDetail,
          query_initiator, account
      );
      if (result != ResultCode::kOk) {
        return result;
      }

      ID iterate_from;
      bool iterate_from_beginning = false;
      if (start_from_key.empty()) {
        if (!start_from_writer.empty()) {
          return ResultCode::kBadRequestPaginationMeta;
        }
        iterate_from_beginning = true;
      } else if (start_from_writer.empty()) {
        return ResultCode::kBadRequestPaginationMeta;
      }

      if (!iterate_from_beginning) {
        iterate_from = acc_details_.makeCompositeKey(
            account_id, start_from_key, start_from_writer);
      }

      bool getting_value = true;
      bool ok = acc_details_.get(
          iterate_from, account_id, key, writer,
          [&](const Slice& a,
              const Slice& k,
              const Slice& w,
              const Slice& v) -> bool
          {
              if (getting_value) {
                value = v.ToString();
                getting_value = false;
                return true;
              }
              next_key = k.ToString();
              next_writer = w.ToString();
              return false;
          },
          total_values
      );

      return ok ? ResultCode::kOk : ResultCode::kNoAccountDetail;
    }

    /*
    ResultCode ImmutableWsv::getAccountTransactions(
        const AccountID& query_initiator_id,
        const AccountID& account_id,
        size_t page_size,

    ) {

    }
     */

    ResultCode ImmutableWsv::loadAccounts(
        const AccountID& query_initiator_id,
        const AccountID& account_id,
        RolePermission allPerm,
        RolePermission myPerm,
        RolePermission domainPerm,
        Account*& query_initiator,
        Account*& account)
    {
      account = nullptr;
      query_initiator = accounts_.getAccount(query_initiator_id);
        if (!query_initiator) {
        return ResultCode::kNoCreatorAccount;
      }
      if (query_initiator_id == account_id) {
        account = query_initiator;
      }
      bool hasPerm = query_initiator->hasPermission(allPerm);
      if (account == query_initiator) {
        hasPerm = query_initiator->hasPermission(myPerm);
      } else {
        account = accounts_.getAccount(account_id);
      }
      if (!account) {
        return hasPerm ? ResultCode::kAccountDoesntExist : ResultCode::kNoPermission;
      }
      if (!hasPerm) {
        hasPerm = (query_initiator->domain_id == account->domain_id &&
                   query_initiator->hasPermission(domainPerm));
      }
      return hasPerm ? ResultCode::kOk : ResultCode::kNoPermission;
    }

    ResultCode ImmutableWsv::loadAccount(
        const AccountID& query_initiator_id,
        RolePermission perm,
        Account*& query_initiator)
    {
      query_initiator = accounts_.getAccount(query_initiator_id);
      if (!query_initiator) {
        return ResultCode::kNoCreatorAccount;
      }
      if (!query_initiator->hasPermission(perm)) {
        return ResultCode::kNoPermission;
      }
      return ResultCode::kOk;
    }

  }  // namespace newstorage
}  // namespace iroha