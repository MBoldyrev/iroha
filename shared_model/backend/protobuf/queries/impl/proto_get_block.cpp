/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "backend/protobuf/queries/proto_get_block.hpp"

GetBlock::GetBlock(iroha::protocol::Query &query)
    : get_block_{query.payload().get_block()} {}

types::HeightType GetBlock::height() const {
  return get_block_.height();
}
