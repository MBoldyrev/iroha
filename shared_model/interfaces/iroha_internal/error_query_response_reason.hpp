/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_ERROR_QUERY_RESPONSE_REASON_HPP
#define IROHA_ERROR_QUERY_RESPONSE_REASON_HPP

namespace shared_model {

  /**
   * Describes type of error to be placed inside the error query response
   */
  enum class QueryErrorType {
    kStatelessFailed,
    kStatefulFailed,
    kNoAccount,
    kNoAccountAssets,
    kNoAccountDetail,
    kNoSignatories,
    kNotSupported,
    kNoAsset,
    kNoRoles
  };

}  // namespace shared_model

#endif
