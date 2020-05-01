/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "interfaces/query_responses/engine_response_record.hpp"

using namespace shared_model::interface;

bool EngineReceipt::operator==(const ModelType &rhs) const {
  return commandIndex() == rhs.commandIndex() and response() == rhs.response();
}

std::string EngineReceipt::toString() const {
  return detail::PrettyStringBuilder()
      .init("EngineReceipt")
      .appendNamed("command_index", commandIndex())
      .appendNamed("reponse", response())
      .finalize();
}
