/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "interfaces/queries/get_peers.hpp"

namespace shared_model {

  std::string GetPeers::toString() const {
    return detail::PrettyStringBuilder().init("GetPeers").finalize();
  }

  bool GetPeers::operator==(const ModelType &rhs) const {
    return true;
  }

}  // namespace shared_model
