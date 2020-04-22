/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_PROTO_ENUM_TO_STRING_HPP
#define IROHA_PROTO_ENUM_TO_STRING_HPP

#include <iostream>

#include <google/protobuf/generated_enum_reflection.h>

#define IROHA_DEFINE_PROTO_ENUM_TO_STRING(EnumType)                         \
  inline std::ostream &operator<<(std::ostream &os, const EnumType &val) {  \
    const ::google::protobuf::EnumDescriptor *const descriptor =            \
        ::google::protobuf::GetEnumDescriptor<EnumType>();                  \
    return os << ::google::protobuf::internal::NameOfEnum(descriptor, val); \
  }

#endif
