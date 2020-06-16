#
# Copyright Soramitsu Co., Ltd. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0
#

set(UTIMACO_CRYPTO_APIS_DIR /home/mikhail/work/utimaco-hsm/Utimaco-Block-safe/Utimaco-Block-safe-Simulator-V2.00.0.0/Software/Linux/x86-64/Crypto_APIs)

add_library(cxi SHARED IMPORTED)
set_target_properties(cxi PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES ${UTIMACO_CRYPTO_APIS_DIR}/CXI/include
  #INTERFACE_LINK_DIRECTORIES ${UTIMACO_CRYPTO_APIS_DIR}/CXI/lib
  IMPORTED_LOCATION ${UTIMACO_CRYPTO_APIS_DIR}/CXI/lib/libcxi.so
  )

add_library(hsm_utimaco
  connection.cpp
  crypto_signer.cpp
  )
target_link_libraries(hsm_utimaco
  common
  cxi
  shared_model_cryptography_model
  )

target_compile_definitions(hsm_utimaco
  PUBLIC
    USE_HSM_UTIMACO
  )
