/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ametsuchi/newstorage/storage_factory.hpp"
#include "ametsuchi/newstorage/storage_impl.hpp"
#include "cryptography/public_key.hpp"
#include "ametsuchi/newstorage/sqlite_wrapper.hpp"
#include "ametsuchi/newstorage/wsv_sqlite_db.hpp"
#include "ametsuchi/newstorage/block_index_db.hpp"
#include "boost/filesystem/operations.hpp"
#include "logger/logger.hpp"
#include "logger/logger_manager.hpp"

namespace iroha {
  namespace newstorage {

    const char *StorageFactory::kWsvDB = "wsv.db";
    const char *StorageFactory::kIndexDB = "index.db";
    const char *StorageFactory::kBlocks = "blocks";
    const char *StorageFactory::kProps = "props";

    namespace fs = boost::filesystem;

    std::shared_ptr<ametsuchi::Storage> StorageFactory::create() {
      fs::path path(options_.path);

      if (options_.overwrite_ledger) {
        fs::remove_all(path);
      } else if (options_.reset_wsv) {
        fs::remove_all(path / kWsvDB);
        fs::remove_all(path / kIndexDB);
        fs::remove_all(path / kProps);
      }

      fs::create_directories(path / kBlocks);
      fs::create_directories(path / kProps);

      SqliteWrapper::Options sqlite_opts;
      sqlite_opts.log_level = logger::LogLevel::kError;

      sqlite_opts.db_file = (path / kWsvDB).string();
      sqlite_opts.log_prefix = "WSV db";
      WsvSqliteDB wsv(SqliteWrapper::create(sqlite_opts,
                                            options_.log_manager->getLogger()));

      if (!options_.alternative_peers.empty()) {
        wsv.dropPeers();

        for (const auto &peer : options_.alternative_peers) {
          if (wsv.addPeer(peer.pubkey().toString(), peer.address()) != 1) {
            throw std::runtime_error(wsv.getLastError());
          }
        }
      }

      return std::make_shared<StorageImpl>(
        std::move(options_), std::move(wsv)
      );
    }

  }  // namespace newstorage
}  // namespace iroha
