/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "network/impl/grpc_client_factory.hpp"

#include <limits>

#include <boost/algorithm/string/join.hpp>
#include <boost/format.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include "common/bind.hpp"
#include "network/impl/grpc_channel_builder_params.hpp"

using namespace std::literals::chrono_literals;

using iroha::operator|;

std::string makeJsonString(const std::string &val) {
  return std::string{"\""} + val + "\"";
}

namespace iroha {
  namespace network {

    std::unique_ptr<GrpcClientParams> getDefaultChannelParams() {
      static const auto retry_policy = [] {
        GrpcClientParams::RetryPolicy retry_policy;
        retry_policy.max_attempts = 5u;
        retry_policy.initial_backoff = 5s;
        retry_policy.max_backoff = 120s;
        retry_policy.backoff_multiplier = 1.6f;
        retry_policy.retryable_status_codes = {"UNKNOWN",
                                               "DEADLINE_EXCEEDED",
                                               "ABORTED",
                                               "INTERNAL",
                                               "UNAVAILABLE"};
      }();
      auto params = std::make_unique<GrpcClientParams>();
      params->max_request_message_bytes = std::numeric_limits<int>::max();
      params->max_response_message_bytes = std::numeric_limits<int>::max();
      params->retry_policy = retry_policy;
      return params;
    }

    grpc::ChannelArguments detail::getChannelArguments(
        const std::string &service_full_name, const GrpcClientParams &params) {
      std::string retry_policy =
          params.retry_policy | [](const auto &retry_policy) {
            auto formatted =
                boost::format(R"(
                "retryPolicy": {
                  "maxAttempts": %d,
                  "initialBackoff": "%ds",
                  "maxBackoff": "%ds",
                  "backoffMultiplier": %f,
                  "retryableStatusCodes": [
                    %s
                  ]
                },)")
                % retry_policy.max_attempts
                % retry_policy.initial_backoff.count()
                % retry_policy.max_backoff.count()
                % retry_policy.backoff_multiplier
                % boost::algorithm::join(
                      retry_policy.retryable_status_codes
                          | boost::adaptors::transformed(makeJsonString),
                      ", ");
            return formatted.str();
          };
      std::string service_config = (boost::format(R"(
            {
              "methodConfig": [ {
                "name": [
                  { "service": "%s" }
                ],
                %s
                "maxRequestMessageBytes": %d,
                "maxResponseMessageBytes": %d
              } ]
            })") % service_full_name % retry_policy
                                    % params.max_request_message_bytes
                                    % params.max_response_message_bytes)
                                       .str();

      std::cerr << std::endl << service_config << std::endl;
      grpc::ChannelArguments args;
      args.SetServiceConfigJSON(service_config);
      return args;
    }

  }  // namespace network
}  // namespace iroha
