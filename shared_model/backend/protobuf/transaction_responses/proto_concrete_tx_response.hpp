/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "backend/protobuf/common_objects/proto_ref.hpp"
#include "endpoint.pb.h"
#include "interfaces/transaction_responses/committed_tx_response.hpp"
#include "interfaces/transaction_responses/enough_signatures_collected_response.hpp"
#include "interfaces/transaction_responses/mst_expired_response.hpp"
#include "interfaces/transaction_responses/mst_pending_response.hpp"
#include "interfaces/transaction_responses/not_received_tx_response.hpp"
#include "interfaces/transaction_responses/rejected_tx_response.hpp"
#include "interfaces/transaction_responses/stateful_failed_tx_response.hpp"
#include "interfaces/transaction_responses/stateful_valid_tx_response.hpp"
#include "interfaces/transaction_responses/stateless_failed_tx_response.hpp"
#include "interfaces/transaction_responses/stateless_valid_tx_response.hpp"
#include "interfaces/transaction_responses/tx_response.hpp"

namespace shared_model {
  // -------------------------| Stateless statuses |--------------------------

  using StatelessFailedTxResponse =
      ProtoRef<StatelessFailedTxResponse, iroha::protocol::ToriiResponse>;
  using StatelessValidTxResponse =
      ProtoRef<StatelessValidTxResponse, iroha::protocol::ToriiResponse>;
  // -------------------------| Stateful statuses |---------------------------

  using StatefulFailedTxResponse =
      ProtoRef<StatefulFailedTxResponse, iroha::protocol::ToriiResponse>;
  using StatefulValidTxResponse =
      ProtoRef<StatefulValidTxResponse, iroha::protocol::ToriiResponse>;

  // ----------------------------| End statuses |-----------------------------

  using CommittedTxResponse =
      ProtoRef<CommittedTxResponse, iroha::protocol::ToriiResponse>;
  using RejectedTxResponse =
      ProtoRef<RejectedTxResponse, iroha::protocol::ToriiResponse>;

  // ---------------------------| Rest statuses |-----------------------------

  using MstExpiredResponse =
      ProtoRef<MstExpiredResponse, iroha::protocol::ToriiResponse>;
  using NotReceivedTxResponse =
      ProtoRef<NotReceivedTxResponse, iroha::protocol::ToriiResponse>;
  using MstPendingResponse =
      ProtoRef<MstPendingResponse, iroha::protocol::ToriiResponse>;
  using EnoughSignaturesCollectedResponse =
      ProtoRef<EnoughSignaturesCollectedResponse,
               iroha::protocol::ToriiResponse>;
}  // namespace shared_model
