/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_RESULT_CODE_HPP
#define IROHA_RESULT_CODE_HPP

namespace iroha {
  namespace newstorage {

    enum class ResultCode {
      kOk,
      kBadRequestPaginationMeta,
      kAccountDoesntExist,
      kNoAccountDetail,
      kPermitteeAccountDoesntExist,
      kRoleDoesntExist,
      kDefaultRoleDoesntExist,
      kDomainDoesntExist,
      kDomainAlreadyExists,
      kAssetAlreadyExists,
      kAssetDoesntExist,
      kRoleAlreadyExists,
      kSignatoryAlreadyExists,
      kAccountAlreadyExists,
      kAccountHasNoRole,
      kNoAccountAsset,
      kNotEnoughBalance,
      kAmountOverflow,
      kPeerAlreadyExists,
      kPeerDoesntExist,
      kNoPermission,
      kNoCreatorAccount,
      kDataIntegrityError
    };
  }
}

#endif  // IROHA_RESULT_CODE_HPP