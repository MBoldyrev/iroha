/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_REL_DB_BACKEND_HPP
#define IROHA_REL_DB_BACKEND_HPP

#include <string>
#include <functional>
#include "logger/logger_fwd.hpp"

namespace sqlite { class database; }

namespace iroha {
    namespace newstorage {

        class RelDbBackend {
        public:
            RelDbBackend(const std::string& db_file, logger::LoggerPtr log);

            void getSignatories(const std::string &account_id, std::function<void(const std::string&)> fn);

            void getPeers(std::function<void(const std::string&, const std::string&)> fn);

        private:
          void createSchema();

            std::shared_ptr<sqlite::database> db_;
            logger::LoggerPtr log_;
        };
    }  // namespace newstorage
}  // namespace iroha

#endif //IROHA_REL_DB_BACKEND_HPP
