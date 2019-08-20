/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_YAC_CRYPTO_PROVIDER_IMPL_HPP
#define IROHA_YAC_CRYPTO_PROVIDER_IMPL_HPP

#include "consensus/yac/yac_crypto_provider.hpp"

#include "yac_transport_export.h"

#include "cryptography/keypair.hpp"

namespace iroha {
  namespace consensus {
    namespace yac {
      class YAC_TRANSPORT_EXPORT CryptoProviderImpl : public YacCryptoProvider {
       public:
        CryptoProviderImpl(const shared_model::crypto::Keypair &keypair);

        bool verify(const std::vector<VoteMessage> &msg) override;

        VoteMessage getVote(YacHash hash) override;

       private:
        shared_model::crypto::Keypair keypair_;
      };
    }  // namespace yac
  }    // namespace consensus
}  // namespace iroha

#endif  // IROHA_YAC_CRYPTO_PROVIDER_IMPL_HPP
