/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sqlite_modern_cpp.h"
#include "ametsuchi/newstorage/rel_db_backend.hpp"
#include <boost/filesystem/operations.hpp>
//#include "logger/logger.hpp"

namespace iroha {
    namespace newstorage {

     RelDbBackend::RelDbBackend(const std::string& db_file, logger::LoggerPtr log) :
        db_(std::make_shared<sqlite::database>(db_file)),
        path_(db_file),
        log_(std::move(log))
     {
        createSchema();
     }

     void RelDbBackend::getSignatories(const std::string &account_id, std::function<void(const std::string&)> fn) {
       *db_ << "select public_key from account_has_signatory where account_id = ?"
          << account_id
          >> fn;
     }

     void RelDbBackend::getPeers(std::function<void(const std::string&, const std::string&)> fn) {
       *db_ << "select * from peer" >> fn;
     }

     void RelDbBackend::insertPeer(const std::string &pub_key, const std::string &address) {
       *db_ << "insert into peer values (?,?)" << pub_key << address;
     }

     void RelDbBackend::dropPeers() {
       *db_ << "delete from peer";
     }

     void RelDbBackend::dropAll() {
       // TODO mutex

       db_.reset();
       boost::filesystem::remove_all(path_);
       db_ = std::make_shared<sqlite::database>(path_);
       createSchema();
     }

     int RelDbBackend::getTxStatusByHash(const std::string &hash) {
       int status = -1;
       *db_ << "select status from tx_status_by_hash where hash = ?" << hash
            >> [&status](int s) { status = s; };
       return status;
     }

     void RelDbBackend::createSchema() {
       static const char* prepare_tables_sql[] = {
        "CREATE TABLE IF NOT EXISTS role (\
            role_id TEXT,\
            PRIMARY KEY (role_id));",
        "CREATE TABLE IF NOT EXISTS domain (\
            domain_id TEXT,\
            default_role TEXT NOT NULL,\
            PRIMARY KEY (domain_id))",
        "CREATE TABLE IF NOT EXISTS signatory (\
            public_key TEXT NOT NULL,\
            PRIMARY KEY (public_key))",
        "CREATE TABLE IF NOT EXISTS account (\
            account_id TEXT,\
            domain_id TEXT,\
            quorum INTEGER NOT NULL,\
            PRIMARY KEY (account_id))",
        "CREATE TABLE IF NOT EXISTS account_has_signatory (\
            account_id TEXT,\
            public_key TEXT NOT NULL,\
            PRIMARY KEY (account_id, public_key))",
        "CREATE TABLE IF NOT EXISTS peer (\
            public_key TEXT NOT NULL,\
            address TEXT NOT NULL UNIQUE,\
            PRIMARY KEY (public_key))",
        "CREATE TABLE IF NOT EXISTS asset (\
            asset_id TEXT,\
            domain_id TEXT NOT NULL,\
            precision int NOT NULL,\
            PRIMARY KEY (asset_id))",
        "CREATE TABLE IF NOT EXISTS account_has_asset (\
            account_id TEXT NOT NULL,\
            asset_id TEXT NOT NULL,\
            amount BLOB NOT NULL,\
            PRIMARY KEY (account_id, asset_id))",
        "CREATE TABLE IF NOT EXISTS role_has_permissions (\
            role_id TEXT NOT NULL,\
            permission BLOB NOT NULL,\
            PRIMARY KEY (role_id))",
        "CREATE TABLE IF NOT EXISTS account_has_roles (\
            account_id TEXT NOT NULL,\
            role_id TEXT NOT NULL,\
            PRIMARY KEY (account_id, role_id))",
        "CREATE TABLE IF NOT EXISTS account_has_grantable_permissions (\
            permittee_account_id TEXT NOT NULL,\
            account_id TEXT NOT NULL,\
            permission BLOB NOT NULL,\
            PRIMARY KEY (permittee_account_id, account_id))",
        "CREATE TABLE IF NOT EXISTS position_by_hash (\
            hash TEXT NOT NULL,\
            height INTEGER,\
            idx INTEGER)",
        "CREATE TABLE IF NOT EXISTS tx_status_by_hash (\
            hash TEXT NOT NULL,\
            status INTEGER)",
        "CREATE INDEX IF NOT EXISTS tx_status_by_hash_hash_index\
          ON tx_status_by_hash\
          (hash)",
        "CREATE TABLE IF NOT EXISTS tx_position_by_creator (\
            creator_id TEXT,\
            height INTEGER,\
            idx INTEGER)",
        "CREATE TABLE IF NOT EXISTS position_by_account_asset (\
            account_id TEXT,\
            asset_id TEXT,\
            height INTEGER,\
            idx INTEGER)",
        "CREATE INDEX IF NOT EXISTS position_by_account_asset_index\
          ON position_by_account_asset\
          (account_id, asset_id, height, idx ASC)"
       };
       //static const size_t num_queries = sizeof(prepare_tables_sql) / sizeof(prepare_tables_sql[0]);
       for (const char* query : prepare_tables_sql) {
         *db_ << query;
       }
     }

    }  // namespace newstorage
}  // namespace iroha


