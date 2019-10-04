/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ametsuchi/newstorage/temporary_wsv_impl.hpp"
#include "ametsuchi/newstorage/command_executor_impl.hpp"
#include "ametsuchi/newstorage/mutable_wsv.hpp"
#include "ametsuchi/tx_executor.hpp"
#include "cryptography/public_key.hpp"
#include "interfaces/commands/command.hpp"
#include "interfaces/permission_to_string.hpp"
#include "interfaces/transaction.hpp"
#include "logger/logger.hpp"
#include "logger/logger_manager.hpp"

namespace iroha {
  namespace newstorage {
    TemporaryWsvImpl::TemporaryWsvImpl(
        MutableWsv& db,
        std::shared_ptr<ametsuchi::CommandExecutor> command_executor,
        logger::LoggerManagerTreePtr log_manager)
        : db_(db),
          transaction_executor_(std::make_unique<ametsuchi::TransactionExecutor>(
              std::move(command_executor))),
          log_manager_(std::move(log_manager)),
          log_(log_manager_->getLogger()) {
      db_tx_ = db_.createTx();
    }

    expected::Result<void, validation::CommandError>
    TemporaryWsvImpl::validateSignatures(
        const shared_model::interface::Transaction &transaction) {

      uint16_t quorum = 0;
      const auto& signatories = db_.loadAccountSignatoriesAndQuorum(
          transaction.creatorAccountId(), quorum);

      if (quorum == 0 || signatories.empty()) {
        auto error_str = "Transaction " + transaction.toString()
            + " failed signatures validation with db error"; // TODO: verbose
        return expected::makeError(validation::CommandError{
            "signatures validation", 2, error_str, false});
      }

      uint16_t count = 0;
      for (const auto& s : transaction.signatures()) {
        if (signatories.count(s.publicKey().hex())) ++count;
      }

      if (count < quorum) {
        auto error_str = "Transaction " + transaction.toString()
                         + " failed signatures validation";
        // TODO [IR-1816] Akvinikym 29.10.18: substitute error code magic number
        // with named constant
        return expected::makeError(validation::CommandError{
            "signatures validation", 2, error_str, false});
      }

      return {};
    }

    expected::Result<void, validation::CommandError> TemporaryWsvImpl::apply(
        const shared_model::interface::Transaction &transaction) {
      auto savepoint = db_.createSavepoint("savepoint_temp_wsv");

      auto result = validateSignatures(transaction);
      if (hasError(result)) {
        return result;
      }

      if (auto error = expected::resultToOptionalError(
          transaction_executor_->execute(transaction, true))) {
        return expected::makeError(
            validation::CommandError{error->command_error.command_name,
                                     error->command_error.error_code,
                                     error->command_error.error_extra,
                                     true,
                                     error->command_index});
      }
        // success
        savepoint.release();
        return {};
    }

    std::unique_ptr<ametsuchi::TemporaryWsv::SavepointWrapper>
    TemporaryWsvImpl::createSavepoint(const std::string &name) {
      return std::make_unique<TemporaryWsvImpl::SavepointWrapperImpl>(
          std::move(db_.createSavepoint(name)),
              log_manager_->getChild("SavepointWrapper")->getLogger());
    }

    TemporaryWsvImpl::~TemporaryWsvImpl() {
      try {
        db_tx_.rollback();
      } catch (std::exception &e) {
        log_->error("Rollback did not happen: {}", e.what());
      }
    }

    TemporaryWsvImpl::SavepointWrapperImpl::SavepointWrapperImpl(
        DbSavepoint savepoint,
        logger::LoggerPtr log)
        : savepoint_(std::move(savepoint)),
          is_released_{false},
          log_(std::move(log))
    {}

    void TemporaryWsvImpl::SavepointWrapperImpl::release() {
      is_released_ = true;
    }

    TemporaryWsvImpl::SavepointWrapperImpl::~SavepointWrapperImpl() {
      try {
        if (not is_released_) {
          savepoint_.rollback();
        } else {
          savepoint_.release();
        }
      } catch (std::exception &e) {
        log_->error("SQL error. Reason: {}", e.what());
      }
    }

  }  // namespace ametsuchi
}  // namespace iroha
