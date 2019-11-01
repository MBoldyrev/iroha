/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_PROTO_UTIL_HPP
#define IROHA_SHARED_MODEL_PROTO_UTIL_HPP

namespace google {
  namespace protobuf {
    class Message;
  }
}  // namespace google

namespace shared_model {
  namespace util {

    // template <typename T>
    // inline crypto::Blob makeBlob(T &&message) {
    //  crypto::Blob::Bytes data;
    //  data.resize(message.ByteSizeLong());
    //  message.SerializeToArray(data.data(), data.size());
    //  return crypto::Blob(std::move(data));
    //}
    crypto::Blob makeBlob(const ::google::protobuf::Message &message);

    using TransactionFactoryType =
        AbstractTransportFactory<shared_model::Transaction,
                                 iroha::protocol::Transaction>;

    iroha::expected::Result<types::SharedTxsCollectionType,
                            TransactionFactoryType::Error>
    deserializeTransactions(
        const TransactionFactoryType &transaction_factory,
        const google::protobuf::RepeatedPtrField<iroha::protocol::Transaction>
            &transactions);

  }  // namespace util
}  // namespace shared_model

#endif  // IROHA_SHARED_MODEL_PROTO_UTIL_HPP
