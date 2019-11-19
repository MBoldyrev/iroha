/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_PROTO_TRANSACTION_BUILDER_TEMPLATE_HPP
#define IROHA_PROTO_TRANSACTION_BUILDER_TEMPLATE_HPP

#include "backend/protobuf/transaction.hpp"

#include <boost/range/algorithm/for_each.hpp>

#include "commands.pb.h"
#include "primitive.pb.h"
#include "transaction.pb.h"

#include "backend/protobuf/permissions.hpp"
#include "builders/protobuf/unsigned_proto.hpp"
#include "interfaces/common_objects/types.hpp"
#include "interfaces/permissions.hpp"
#include "module/irohad/common/validators_config.hpp"
#include "validators/default_validator.hpp"

namespace shared_model {
  namespace proto {

    /**
     * Template tx builder for creating new types of transaction builders by
     * means of replacing template parameters
     * @tparam S -- field counter for checking that all required fields are set
     * @tparam SV -- stateless validator called when build method is invoked
     * @tparam BT -- build type of built object returned by build method
     */
    template <int S = 0,
              typename SV = validation::DefaultUnsignedTransactionValidator,
              typename BT = UnsignedWrapper<Transaction>>
    class [[deprecated]] TemplateTransactionBuilder {
     private:
      template <int, typename, typename>
      friend class TemplateTransactionBuilder;

      enum RequiredFields {
        Command,
        CreatorAccountId,
        CreatedTime,
        Quorum,
        TOTAL
      };

      template <int s>
      using NextBuilder = TemplateTransactionBuilder<S | (1 << s), SV, BT>;

      using ProtoTx = iroha::protocol::Transaction;
      using ProtoCommand = iroha::protocol::Command;

      template <int Sp>
      TemplateTransactionBuilder(
          const TemplateTransactionBuilder<Sp, SV, BT> &o)
          : transaction_(o.transaction_),
            stateless_validator_(o.stateless_validator_) {}

      /**
       * Make transformation on copied content
       * @tparam Transformation - callable type for changing the copy
       * @param f - transform function for proto object
       * @return new builder with updated state
       */
      template <int Fields, typename Transformation>
      auto transform(Transformation t) const {
        NextBuilder<Fields> copy = *this;
        t(copy.transaction_);
        return copy;
      }

      /**
       * Make add command transformation on copied object
       * @tparam Transformation - callable type for changing command
       * @param f - transform function for proto command
       * @return new builder with added command
       */
      template <typename Transformation>
      auto addCommand(Transformation t) const {
        NextBuilder<Command> copy = *this;
        t(copy.transaction_.mutable_payload()
              ->mutable_reduced_payload()
              ->add_commands());
        return copy;
      }

      TemplateTransactionBuilder(const SV &validator)
          : stateless_validator_(validator) {}

     public:
      // we do such default initialization only because it is deprecated and
      // used only in tests
      TemplateTransactionBuilder()
          : TemplateTransactionBuilder(
                SV(iroha::test::kTestsValidatorsConfig)) {}

      auto creatorAccountId(const types::AccountIdType &account_id) const {
        return transform<CreatorAccountId>([&](auto &tx) {
          tx.mutable_payload()
              ->mutable_reduced_payload()
              ->set_creator_account_id(account_id);
        });
      }

      auto batchMeta(types::BatchType type, std::vector<types::HashType> hashes)
          const {
        return transform<0>([&](auto &tx) {
          tx.mutable_payload()->mutable_batch()->set_type(
              static_cast<
                  iroha::protocol::Transaction::Payload::BatchMeta::BatchType>(
                  type));
          for (const auto &hash : hashes) {
            tx.mutable_payload()->mutable_batch()->add_reduced_hashes(
                hash.hex());
          }
        });
      }

      auto createdTime(types::TimestampType created_time) const {
        return transform<CreatedTime>([&](auto &tx) {
          tx.mutable_payload()->mutable_reduced_payload()->set_created_time(
              created_time);
        });
      }

      auto quorum(types::QuorumType quorum) const {
        return transform<Quorum>([&](auto &tx) {
          tx.mutable_payload()->mutable_reduced_payload()->set_quorum(quorum);
        });
      }

      auto addAssetQuantity(const types::AssetIdType &asset_id,
                            const std::string &amount) const {
        return addCommand([&](auto proto_command) {
          auto command = proto_command->mutable_add_asset_quantity();
          command->set_asset_id(asset_id);
          command->set_amount(amount);
        });
      }

      auto addPeerRaw(
          const types::AddressType &address,
          const std::string &peer_key,
          const boost::optional<types::TLSCertificateType> &tls_certificate =
              boost::none) const {
        return addCommand([&](auto proto_command) {
          auto command = proto_command->mutable_add_peer();
          auto peer = command->mutable_peer();
          peer->set_address(address);
          peer->set_peer_key(peer_key);
          if (tls_certificate) {
            peer->set_tls_certificate(*tls_certificate);
          }
        });
      }

      auto addPeer(
          const types::AddressType &address,
          const types::PubkeyType &peer_key,
          const boost::optional<types::TLSCertificateType> &tls_certificate =
              boost::none) const {
        return addPeerRaw(address, peer_key.hex(), tls_certificate);
      }

      auto removePeer(const types::PubkeyType &public_key) const {
        return addCommand([&](auto proto_command) {
          auto command = proto_command->mutable_remove_peer();
          command->set_public_key(public_key.hex());
        });
      }

      auto addSignatoryRaw(const types::AccountIdType &account_id,
                           const std::string &public_key) const {
        return addCommand([&](auto proto_command) {
          auto command = proto_command->mutable_add_signatory();
          command->set_account_id(account_id);
          command->set_public_key(public_key);
        });
      }

      auto addSignatory(const types::AccountIdType &account_id,
                        const types::PubkeyType &public_key) const {
        return addSignatoryRaw(account_id, public_key.hex());
      }

      auto removeSignatoryRaw(const types::AccountIdType &account_id,
                              const std::string &public_key) const {
        return addCommand([&](auto proto_command) {
          auto command = proto_command->mutable_remove_signatory();
          command->set_account_id(account_id);
          command->set_public_key(public_key);
        });
      }

      auto removeSignatory(const types::AccountIdType &account_id,
                           const types::PubkeyType &public_key) const {
        return removeSignatoryRaw(account_id, public_key.hex());
      }

      auto appendRole(const types::AccountIdType &account_id,
                      const types::RoleIdType &role_name) const {
        return addCommand([&](auto proto_command) {
          auto command = proto_command->mutable_append_role();
          command->set_account_id(account_id);
          command->set_role_name(role_name);
        });
      }

      auto createAsset(const types::AssetNameType &asset_name,
                       const types::DomainIdType &domain_id,
                       types::PrecisionType precision) const {
        return addCommand([&](auto proto_command) {
          auto command = proto_command->mutable_create_asset();
          command->set_asset_name(asset_name);
          command->set_domain_id(domain_id);
          command->set_precision(precision);
        });
      }

      auto createAccountRaw(const types::AccountNameType &account_name,
                            const types::DomainIdType &domain_id,
                            const std::string &main_pubkey) const {
        return addCommand([&](auto proto_command) {
          auto command = proto_command->mutable_create_account();
          command->set_account_name(account_name);
          command->set_domain_id(domain_id);
          command->set_public_key(main_pubkey);
        });
      }

      auto createAccount(const types::AccountNameType &account_name,
                         const types::DomainIdType &domain_id,
                         const types::PubkeyType &main_pubkey) const {
        return createAccountRaw(account_name, domain_id, main_pubkey.hex());
      }

      auto createDomain(const types::DomainIdType &domain_id,
                        const types::RoleIdType &default_role) const {
        return addCommand([&](auto proto_command) {
          auto command = proto_command->mutable_create_domain();
          command->set_domain_id(domain_id);
          command->set_default_role(default_role);
        });
      }

      auto createRole(const types::RoleIdType &role_name,
                      const RolePermissionSet &permissions) const {
        return addCommand([&](auto proto_command) {
          auto command = proto_command->mutable_create_role();
          command->set_role_name(role_name);
          for (size_t i = 0; i < permissions.size(); ++i) {
            auto perm = static_cast<permissions::Role>(i);
            if (permissions.isSet(perm)) {
              command->add_permissions(permissions::toTransport(perm));
            }
          }
        });
      }

      auto detachRole(const types::AccountIdType &account_id,
                      const types::RoleIdType &role_name) const {
        return addCommand([&](auto proto_command) {
          auto command = proto_command->mutable_detach_role();
          command->set_account_id(account_id);
          command->set_role_name(role_name);
        });
      }

      auto grantPermission(const types::AccountIdType &account_id,
                           permissions::Grantable permission) const {
        return addCommand([&](auto proto_command) {
          auto command = proto_command->mutable_grant_permission();
          command->set_account_id(account_id);
          command->set_permission(permissions::toTransport(permission));
        });
      }

      auto revokePermission(const types::AccountIdType &account_id,
                            permissions::Grantable permission) const {
        return addCommand([&](auto proto_command) {
          auto command = proto_command->mutable_revoke_permission();
          command->set_account_id(account_id);
          command->set_permission(permissions::toTransport(permission));
        });
      }

      auto setAccountDetail(const types::AccountIdType &account_id,
                            const types::AccountDetailKeyType &key,
                            const types::AccountDetailValueType &value) const {
        return addCommand([&](auto proto_command) {
          auto command = proto_command->mutable_set_account_detail();
          command->set_account_id(account_id);
          command->set_key(key);
          command->set_value(value);
        });
      }

      auto setAccountQuorum(const types::AddressType &account_id,
                            types::QuorumType quorum) const {
        return addCommand([&](auto proto_command) {
          auto command = proto_command->mutable_set_account_quorum();
          command->set_account_id(account_id);
          command->set_quorum(quorum);
        });
      }

      auto subtractAssetQuantity(const types::AssetIdType &asset_id,
                                 const std::string &amount) const {
        return addCommand([&](auto proto_command) {
          auto command = proto_command->mutable_subtract_asset_quantity();
          command->set_asset_id(asset_id);
          command->set_amount(amount);
        });
      }

      auto transferAsset(const types::AccountIdType &src_account_id,
                         const types::AccountIdType &dest_account_id,
                         const types::AssetIdType &asset_id,
                         const types::DescriptionType &description,
                         const std::string &amount) const {
        return addCommand([&](auto proto_command) {
          auto command = proto_command->mutable_transfer_asset();
          command->set_src_account_id(src_account_id);
          command->set_dest_account_id(dest_account_id);
          command->set_asset_id(asset_id);
          command->set_description(description);
          command->set_amount(amount);
        });
      }

      auto build() const {
        static_assert(S == (1 << TOTAL) - 1, "Required fields are not set");
        auto result = Transaction(iroha::protocol::Transaction(transaction_));
        auto answer = stateless_validator_.validate(result);
        if (answer.hasErrors()) {
          throw std::invalid_argument(answer.reason());
        }
        return BT(std::move(result));
      }

      static const int total = RequiredFields::TOTAL;

     private:
      ProtoTx transaction_;
      SV stateless_validator_;
    };

  }  // namespace proto
}  // namespace shared_model

#endif  // IROHA_PROTO_TRANSACTION_BUILDER_TEMPLATE_HPP
