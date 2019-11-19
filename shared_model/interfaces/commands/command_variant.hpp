/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_COMMAND_VARIANT_HPP
#define IROHA_SHARED_MODEL_COMMAND_VARIANT_HPP

#include "compare_and_set_account_detail.hpp"
#include "interfaces/commands/command.hpp"

#include <boost/variant.hpp>

namespace boost {
  extern template class variant<
      const shared_model::AddAssetQuantity &,
      const shared_model::AddPeer &,
      const shared_model::AddSignatory &,
      const shared_model::AppendRole &,
      const shared_model::CreateAccount &,
      const shared_model::CreateAsset &,
      const shared_model::CreateDomain &,
      const shared_model::CreateRole &,
      const shared_model::DetachRole &,
      const shared_model::GrantPermission &,
      const shared_model::RemoveSignatory &,
      const shared_model::RevokePermission &,
      const shared_model::SetAccountDetail &,
      const shared_model::SetQuorum &,
      const shared_model::SubtractAssetQuantity &,
      const shared_model::TransferAsset &,
      const shared_model::CompareAndSetAccountDetail &,
      const shared_model::SetSettingValue &>;
}  // namespace boost

#endif  // IROHA_SHARED_MODEL_COMMAND_VARIANT_HPP
