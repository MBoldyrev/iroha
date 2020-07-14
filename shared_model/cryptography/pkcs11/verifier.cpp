/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "cryptography/pkcs11/verifier.hpp"

#include <botan/p11.h>
#include <botan/p11_slot.h>
#include <botan/pk_ops.h>
#include <fmt/format.h>
#include "common/bind.hpp"
#include "common/hexutils.hpp"
#include "common/result.hpp"
#include "cryptography/pkcs11/algorithm_identifier.hpp"
#include "interfaces/common_objects/byte_range.hpp"
#include "multihash/type.hpp"

using namespace shared_model::crypto::pkcs11;
using namespace shared_model::interface::types;

using iroha::operator|;

Verifier::Verifier(OperationContextFactory operation_context_factory)
    : operation_context_factory_(std::move(operation_context_factory)) {
  auto operation_context = operation_context_factory_();
  for (Botan::PKCS11::MechanismType mech :
       operation_context.slot.get_mechanism_list()) {
    getMultihashType(mech) | [&](iroha::multihash::Type type) {
      if (operation_context.slot.get_mechanism_info(mech).flags & CKA_VERIFY) {
        supported_types_.emplace_back();
      }
    };
  }

  Botan::PKCS11::Info module_info = operation_context.module.get_info();
  Botan::PKCS11::SlotInfo slot_info = operation_context.slot.get_slot_info();
  description_ = fmt::format(
      "PKCS11 cryptographic verifier "
      "using library {} version {}.{} from {}, "
      "slot {}",
      module_info.libraryDescription,
      module_info.libraryVersion.major,
      module_info.libraryVersion.minor,
      module_info.manufacturerID,
      slot_info.slotDescription);
}

Verifier::~Verifier() = default;

iroha::expected::Result<void, std::string> Verifier::verify(
    iroha::multihash::Type type,
    shared_model::interface::types::SignatureByteRangeView signature,
    shared_model::interface::types::ByteRange message,
    shared_model::interface::types::PublicKeyByteRangeView public_key) const {
  auto opt_emsa_name = getEmsaName(type);
  auto opt_pkcs11_pubkey = makePublicKeyOfType(type, public_key);
  assert(opt_emsa_name);
  assert(opt_pkcs11_pubkey);
  if (not opt_emsa_name or not opt_pkcs11_pubkey) {
    return iroha::expected::makeError("Unsupported algorithm.");
  }

  std::unique_ptr<Botan::PK_Ops::Verification> pkcs11_verifier =
      opt_pkcs11_pubkey.value()->create_verification_op(opt_emsa_name.value(),
                                                        {});

  ByteRange signature_raw{signature};
  if (pkcs11_verifier->is_valid_signature(
          reinterpret_cast<uint8_t const *>(signature_raw.data()),
          signature_raw.size())) {
    return iroha::expected::Value<void>{};
  }

  return iroha::expected::makeError("Wrong signature.");
}

std::vector<iroha::multihash::Type> Verifier::getSupportedTypes() const {
  return supported_types_;
}