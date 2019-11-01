/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_COMMAND_HPP
#define IROHA_SHARED_MODEL_COMMAND_HPP

#include "interfaces/base/model_primitive.hpp"

#include <boost/variant/variant_fwd.hpp>

namespace shared_model {

  class AddAssetQuantity;
  class AddPeer;
  class AddSignatory;
  class AppendRole;
  class CreateAccount;
  class CreateAsset;
  class CreateDomain;
  class CreateRole;
  class DetachRole;
  class GrantPermission;
  class RemovePeer;
  class RemoveSignatory;
  class RevokePermission;
  class SetAccountDetail;
  class SetQuorum;
  class SubtractAssetQuantity;
  class TransferAsset;
  class CompareAndSetAccountDetail;
  class SetSettingValue;

  /**
   * Class provides commands container for all commands in system.
   * General note: this class is container for commands, not a base class.
   */
  class Command : public ModelPrimitive<Command> {
    /// const reference shortcut type
    template <typename... Value>
    using ConstRefVariant = boost::variant<const Value &...>;

   public:
    using TransportType = iroha::protocol::Command;

    using CommandVariantType = ConstRefVariant<AddAssetQuantity,
                                               AddPeer,
                                               AddSignatory,
                                               AppendRole,
                                               CreateAccount,
                                               CreateAsset,
                                               CreateDomain,
                                               CreateRole,
                                               DetachRole,
                                               GrantPermission,
                                               RemoveSignatory,
                                               RevokePermission,
                                               SetAccountDetail,
                                               SetQuorum,
                                               SubtractAssetQuantity,
                                               TransferAsset,
                                               RemovePeer,
                                               CompareAndSetAccountDetail,
                                               SetSettingValue>;

    explicit Command(TransportType &ref);

    ~Command();

    /**
     * @return reference to const variant with concrete command
     */
    const CommandVariantType &get() const;

    // ------------------------| Primitive override |-------------------------

    std::string toString() const override;

    bool operator==(const ModelType &rhs) const override;

   private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
  };

}  // namespace shared_model

#endif  // IROHA_SHARED_MODEL_COMMAND_HPP
