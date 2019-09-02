/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_GRPC_CLIENT_FACTORY_LRU_HPP
#define IROHA_GRPC_CLIENT_FACTORY_LRU_HPP

#include "network/impl/grpc_client_factory.hpp"

#include "cache/cache.hpp"

namespace iroha {
  namespace network {

    template <typename Service>
    class ClientFactoryLru : public ClientFactory<Service> {
      ClientFactoryLru(const GrpcClientParams &params, std::size_t cache_size)
          : ClientFactory(params),
            insecure_clients_cache_(cache_size, cache_size) {}

      /**
       * Gets an insecure client from cache or creates it.
       * @param address ip address for connection, ipv4:port
       * @return gRPC stub of parametrized type
       */
      std::unique_ptr<Stub> getClient(const grpc::string &address) override {
        return insecure_clients_cache_.getOrCreate(address, [this] {
          return this->ClientFactory<Service>::createClient(address);
        });
      }

     private:
      iroha::cache::Cache<grpc::string, std::unique_ptr<Stub>>
          insecure_clients_cache_;
    };

  }  // namespace network
}  // namespace iroha

#endif  // IROHA_GRPC_CLIENT_FACTORY_LRU_HPP
