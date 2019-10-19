/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_TEMPORARY_WSV_IMPL_HPP
#define IROHA_TEMPORARY_WSV_IMPL_HPP

#include "ametsuchi/temporary_wsv.hpp"
#include "ametsuchi/command_executor.hpp"
#include "ametsuchi/newstorage/db_tx.hpp"
#include "logger/logger_fwd.hpp"
#include "logger/logger_manager_fwd.hpp"

namespace shared_model {
  namespace interface {
    class PermissionToString;
  }
}  // namespace shared_model

namespace iroha {

  namespace ametsuchi {
    class TransactionExecutor;
    class CommandExecutor;
  }

  namespace newstorage {
    class MutableWsv;


    class TemporaryWsvImpl : public ametsuchi::TemporaryWsv {
     public:
      struct SavepointWrapperImpl : public TemporaryWsv::SavepointWrapper {
        SavepointWrapperImpl(DbSavepoint savepoint,
                             logger::LoggerPtr log);

        void release() override;

        ~SavepointWrapperImpl() override;

       private:
        DbSavepoint savepoint_;
        bool is_released_;
        logger::LoggerPtr log_;
      };

      TemporaryWsvImpl(
          MutableWsv& db,
          std::shared_ptr<ametsuchi::CommandExecutor> command_executor,
          logger::LoggerManagerTreePtr log_manager);

      expected::Result<void, validation::CommandError> apply(
          const shared_model::interface::Transaction &transaction) override;

      std::unique_ptr<TemporaryWsv::SavepointWrapper> createSavepoint(
          const std::string &name) override;

      bool rollback() override;

      ~TemporaryWsvImpl() override;

     private:
      /**
       * Verifies whether transaction has at least quorum signatures and they
       * are a subset of creator account signatories
       */
      expected::Result<void, validation::CommandError> validateSignatures(
          const shared_model::interface::Transaction &transaction);

      MutableWsv& db_;
      std::unique_ptr<ametsuchi::TransactionExecutor> transaction_executor_;
      logger::LoggerManagerTreePtr log_manager_;
      logger::LoggerPtr log_;
      DbTransaction db_tx_;
    };
  }  // namespace newstorage
}  // namespace iroha

#endif  // IROHA_TEMPORARY_WSV_IMPL_HPP
