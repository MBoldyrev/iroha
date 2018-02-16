/**
 * Copyright Soramitsu Co., Ltd. 2017 All Rights Reserved.
 * http://soramitsu.co.jp
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "module/irohad/ametsuchi/ametsuchi_mocks.hpp"
#include "module/irohad/validation/validation_mocks.hpp"

#include "framework/test_subscriber.hpp"
#include "model/queries/responses/error_response.hpp"
#include "model/query_execution.hpp"
#include "network/ordering_gate.hpp"
#include "torii/processor/query_processor_impl.hpp"

#include "backend/protobuf/query_responses/proto_error_query_response.hpp"
#include "cryptography/crypto_provider/crypto_defaults.hpp"
#include "cryptography/keypair.hpp"
#include "module/shared_model/builders/protobuf/test_query_builder.hpp"

using namespace iroha;
using namespace iroha::ametsuchi;
using namespace iroha::validation;
using namespace framework::test_subscriber;

using ::testing::_;
using ::testing::A;
using ::testing::Return;

class QueryProcessorTest : public ::testing::Test {
 public:
  void SetUp() override {
    created_time = iroha::time::now();
    account_id = "account@domain";
    counter = 1048576;
  }

  decltype(iroha::time::now()) created_time;
  std::string account_id;
  uint64_t counter;
  shared_model::crypto::Keypair keypair =
      shared_model::crypto::DefaultCryptoAlgorithmType::generateKeypair();
};

/**
 * @given account, ametsuchi queries and query processing factory
 * @when stateless validation error
 * @then Query Processor should return ErrorQueryResponse
 */
TEST_F(QueryProcessorTest, QueryProcessorWhereInvokeInvalidQuery) {
  auto wsv_queries = std::make_shared<MockWsvQuery>();
  auto block_queries = std::make_shared<MockBlockQuery>();
  auto qpf = std::make_unique<model::QueryProcessingFactory>(wsv_queries,
                                                             block_queries);

  auto validation = std::make_shared<MockStatelessValidator>();
  EXPECT_CALL(*validation, validate(A<const model::Query &>()))
      .WillRepeatedly(Return(false));

  iroha::torii::QueryProcessorImpl qpi(std::move(qpf), validation);

  auto query = TestUnsignedQueryBuilder()
                   .createdTime(created_time)
                   .creatorAccountId(account_id)
                   .getAccount(account_id)
                   .queryCounter(counter)
                   .build()
                   .signAndAddSignature(keypair);

  auto wrapper = make_test_subscriber<CallExact>(qpi.queryNotifier(), 1);
  wrapper.subscribe([](auto response) {
    auto resp = response->get();
    // TODO: change to non static assert
    ASSERT_EQ(resp.which(), 3);
  });
  qpi.queryHandle(
      shared_model::detail::makePolymorphic<shared_model::proto::Query>(
          query.getTransport()));
}
