/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_GRPC_CHANNEL_BUILDER_HPP
#define IROHA_GRPC_CHANNEL_BUILDER_HPP

#include <memory>

#include <grpc++/grpc++.h>
#include <boost/optional/optional.hpp>

namespace iroha {
  namespace network {

    struct GrpcClientParams;

    namespace detail {
      grpc::ChannelArguments getChannelArguments(
          const std::string &service_full_name, const GrpcClientParams &params);
    }

    /**
     * Creates client params which enable sending and receiving messages of
     * INT_MAX bytes size
     */
    std::unique_ptr<GrpcClientParams> getDefaultChannelParams();

    /**
     * Creates channel arguments out of provided params.
     * @tparam T type for gRPC stub, e.g. proto::Yac
     * @param params grpc channel params
     * @return gRPC channel arguments
     */
    template <typename T>
    grpc::ChannelArguments getChannelArguments(const GrpcClientParams &params) {
      return getChannelArguments(T::service_full_name(), params);
    }

    template <typename Service>
    class ClientFactory {
      using Stub = typename Service::Stub;
      ClientFactory(const GrpcClientParams &params)
          : channel_args_(getChannelArguments<Service>(params)) {}

      /**
       * Creates an insecure client.
       * @param address ip address for connection, ipv4:port
       * @return gRPC client stub
       */
      virtual std::unique_ptr<Stub> getClient(const grpc::string &address) {
        return Service::NewStub(grpc::CreateCustomChannel(
            address, grpc::InsecureChannelCredentials(), channel_args_));
      }

      /**
       * Creates a client with SSL transport and server certificate check.
       * @param address ip address for connection, ipv4:port
       * @param root_certificate root certificate for the server's CA
       * @return gRPC client stub
       */
      virtual std::unique_ptr<Stub> getSecureClient(
          const grpc::string &address, const std::string &root_certificate) {
        auto options = grpc::SslCredentialsOptions();
        options.pem_root_certs = root_certificate;
        auto credentials = grpc::SslCredentials(options);

        return Service::NewStub(
            grpc::CreateCustomChannel(address, credentials, channel_args_));
      }

     private:
      std::shared_ptr<const GrpcClientParams> params_;
      grpc::ChannelArguments channel_args_;
    };
  }  // namespace network
}  // namespace iroha

#endif  // IROHA_GRPC_CHANNEL_BUILDER_HPP
