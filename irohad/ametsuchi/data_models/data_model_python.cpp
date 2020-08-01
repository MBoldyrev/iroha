/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ametsuchi/data_models/data_model_python.hpp"

#include <pybind11/buffer_info.h>
#include <pybind11/embed.h>
#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>

#include <memory>
#include <rxcpp/rx-lite.hpp>
#include "qry_responses.pb.h"

namespace py = pybind11;
using namespace iroha::ametsuchi;

namespace {
  char const *kPythonInitializeFunctionName = "initialize";
  char const *kPythonGetSupportedDmIdsFunctionName =
      "get_supported_data_model_ids";
  char const *kPythonExecuteFunctionName = "execute";
  char const *kPythonCommitTxFunctionName = "commit_transaction";
  char const *kPythonCommitBlockFunctionName = "commit_block";
  char const *kPythonRollbackTxFunctionName = "rollback_transaction";
  char const *kPythonRollbackBlockFunctionName = "rollback_block";
  char const *kPythonQueryFunctionName = "query";

  iroha::protocol::ModelQueryResponse makeQueryErrorResponse(
      iroha::protocol::ErrorResponse::Reason,
      std::string message,
      uint32_t error_code) {
    // TODO
  }
}  // namespace

struct DataModelPython::Impl {
  py::module python_module;
  py::function func_execute;
  py::function func_commit_tx;
  py::function func_commit_block;
  py::function func_rollback_tx;
  py::function func_rollback_block;
  py::function func_query;
};

DataModelPython::DataModelPython(std::vector<std::string> python_paths,
                                 std::string const &module_name,
                                 std::string const &initialization_argument) {
  py::initialize_interpreter();

  for (auto &&path : python_paths) {
    py::list{py::module::import("sys").attr("path")}.append(std::move(path));
  }

  impl_ = std::make_unique<Impl>();
  Impl &impl = *impl_;

  impl.python_module = py::module::import(module_name.c_str());

  impl.python_module.attr(kPythonInitializeFunctionName)(
      initialization_argument);

  for (auto const &py_dm_id :
       impl.python_module.attr(kPythonGetSupportedDmIdsFunctionName)()) {
    auto py_dm_id_tuple = py_dm_id.cast<py::tuple>();
    supported_dm_ids_.emplace_back(shared_model::interface::DataModelId{
        py_dm_id_tuple[0].cast<std::string>(),
        py_dm_id_tuple[1].cast<std::string>()});
  }

  auto init_python_func = [&impl](auto &func, auto const &func_name) {
    func = py::function{impl.python_module.attr(func_name)};
  };

  init_python_func(impl.func_execute, kPythonExecuteFunctionName);
  init_python_func(impl.func_commit_tx, kPythonCommitTxFunctionName);
  init_python_func(impl.func_commit_block, kPythonCommitBlockFunctionName);
  init_python_func(impl.func_rollback_tx, kPythonRollbackTxFunctionName);
  init_python_func(impl.func_rollback_block, kPythonRollbackBlockFunctionName);
  init_python_func(impl.func_query, kPythonQueryFunctionName);
}

DataModelPython::~DataModelPython() {
  impl_.reset();
  py::finalize_interpreter();
}

CommandResult DataModelPython::execute(
    shared_model::proto::CallModel const &cmd) {
  std::string const cmd_str{cmd.getTransport().SerializeAsString()};

  try {
    py::memoryview cmd_mem_view{py::memoryview::from_memory(
        cmd_str.data(), static_cast<long>(cmd_str.size()))};

    py::object py_result_obj = impl_->func_execute(cmd_mem_view);

    if (not py_result_obj.is_none()) {
      CommandError result;

      py::tuple py_result_tuple{py_result_obj};
      if (py_result_tuple.size() != 2) {
        throw std::runtime_error{"execution result has wrong format"};
      }

      result.command_name = cmd.toString();
      result.error_code = py_result_tuple[0].cast<size_t>();
      result.error_extra.assign(py_result_tuple[1].cast<std::string>());

      return result;
    }

    return iroha::expected::Value<void>{};
  } catch (std::runtime_error const &e) {
    return CommandError{cmd.toString(), 1, e.what()};
  }
}

void DataModelPython::commitTransaction() {
  impl_->func_commit_tx();
}

void DataModelPython::commitBlock() {
  impl_->func_commit_block();
}

void DataModelPython::rollbackTransaction() {
  impl_->func_rollback_tx();
}

void DataModelPython::rollbackBlock() {
  impl_->func_rollback_block();
}

QueryResult DataModelPython::query(
    shared_model::proto::QueryModel const &qry) const {
  return rxcpp::observable<>::create<iroha::protocol::ModelQueryResponse>(
      [qry_str{qry.getTransport().SerializeAsString()}](auto subscriber) {
        try {
          py::memoryview qry_mem_view{py::memoryview::from_memory(
              qry_str.data(), static_cast<long>(qry_str.size()))};

          for (auto py_val : impl_->func_query(qry_mem_view)) {
            if (not subscriber.is_subscribed()) {
              break;
            }
            py::buffer_info py_val_mem_view{py::buffer{py_val}.request()};
            iroha::protocol::QueryModelResponse proto_val;
            proto_val.ParseFromArray(py_val_mem_view.ptr, py_val_mem_view.size);
            subscriber.on_next(std::move(proto_val));
          }
          subscriber.on_completed();
        } catch (std::runtime_error const &e) {
          subscriber.on_next(makeQueryErrorResponse(1, e.what()));
          subscriber.on_completed();
        }
      });
}

std::vector<shared_model::interface::DataModelId>
DataModelPython::getSupportedDataModelIds() const {
  return supported_dm_ids_;
}
