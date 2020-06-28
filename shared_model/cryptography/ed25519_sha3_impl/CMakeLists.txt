#
# Copyright Soramitsu Co., Ltd. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0
#

add_library(custom_ed25519 STATIC IMPORTED)
set_target_properties(custom_ed25519
  PROPERTIES
    IMPORTED_LOCATION
      #/home/mikhail/work/iroha-ed25519/build/libed25519_cpp.a
      /home/mikhail/work/iroha-ed25519/build/libed25519_cpp.so
  )
target_include_directories(custom_ed25519
  INTERFACE
    /home/mikhail/work/iroha-ed25519/facades
    /home/mikhail/work/iroha-ed25519/facades/ed25519_cpp
    /home/mikhail/work/iroha-ed25519/build
  )

add_subdirectory(internal)

add_library(sha3_cryptography
    crypto_provider.cpp
    signer.cpp
    verifier.cpp
    )
target_link_libraries(sha3_cryptography
    common
    custom_ed25519
    shared_model_cryptography_model
    )
target_include_directories(sha3_cryptography
  PRIVATE
    /home/mikhail/work/iroha-ed25519/facades
    /home/mikhail/work/iroha-ed25519/facades/ed25519_cpp
    /home/mikhail/work/iroha-ed25519/build
  )
