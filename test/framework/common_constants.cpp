/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "framework/common_constants.hpp"

#include "cryptography/crypto_provider/crypto_defaults.hpp"

using namespace shared_model::crypto;

namespace common_constants {

  // user names
  const std::string kAdminName = "admin";
  const std::string kUser = "user";
  const std::string kAnotherUser = "user2";

  // role names
  const std::string kAdminRole = "admin_role";
  const std::string kDefaultRole = "default_role";
  const std::string kRole = "user_role";

  // asset names
  const std::string kAssetName = "coin";

  // domain names
  const std::string kDomain = "domain";
  const std::string kSecondDomain = "domain2";

  // ids
  const std::string kAdminId = kAdminName + "@" + kDomain;
  const std::string kUserId = kUser + "@" + kDomain;
  const std::string kSameDomainUserId = kAnotherUser + "@" + kDomain;
  const std::string kAnotherDomainUserId = kAnotherUser + "@" + kSecondDomain;
  const std::string kAssetId = kAssetName + "#" + kDomain;
  const std::string kAnotherDomainAssetId = kAssetName + "#" + kSecondDomain;

  // keypairs
  const Keypair kAdminKeypair = DefaultCryptoAlgorithmType::generateKeypair();
  const Keypair kUserKeypair = DefaultCryptoAlgorithmType::generateKeypair();
  const Keypair kSameDomainUserKeypair =
      DefaultCryptoAlgorithmType::generateKeypair();
  const Keypair kAnotherDomainUserKeypair =
      DefaultCryptoAlgorithmType::generateKeypair();

  // misc
  const shared_model::interface::Amount kAmountPrec1Max{
      "5789604461865809771178549250434395392663"
      "4992332820282019728792003956564819967.9"};  // 2**(256 - 1) - 10**(-1)
  const shared_model::interface::Amount kAmountPrec2Max{
      "2894802230932904885589274625217197696331"
      "7496166410141009864396001978282409983.99"};  // 2**(256 - 2) - 10**(-2)
}  // namespace common_constants
