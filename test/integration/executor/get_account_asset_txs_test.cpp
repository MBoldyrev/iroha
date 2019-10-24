/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "integration/executor/executor_fixture.hpp"

#include <gtest/gtest.h>
#include <boost/format.hpp>
#include "backend/protobuf/queries/proto_query.hpp"
#include "framework/common_constants.hpp"
#include "integration/executor/query_permission_test.hpp"
#include "interfaces/query_responses/transactions_page_response.hpp"
#include "interfaces/transaction.hpp"
#include "module/shared_model/mock_objects_factories/mock_command_factory.hpp"
#include "module/shared_model/mock_objects_factories/mock_query_factory.hpp"

using namespace common_constants;
using namespace executor_testing;
using namespace framework::expected;
using namespace shared_model::interface::types;

using iroha::ametsuchi::Indexer;
using iroha::ametsuchi::QueryExecutorResult;
using shared_model::interface::TransactionsPageResponse;
using shared_model::interface::permissions::Role;

static const HeightType kHeight = 1;

struct GetAccountAssetTransactionsTest : public ExecutorTestBase {
  static HashType makeTxHash(size_t n) {
    return HashType(std::to_string(n));
  }

  /**
   * Generates the given amount of tx hashes and indexes them as asset
   * transactions.
   */
  void addAssetTxs(size_t n) {
    SCOPED_TRACE("addAssetTxs");
    createAsset(kAssetName, kDomain, 1);
    for (size_t i = 0; i < n; ++i) {
      auto hash = makeTxHash(i);
      getItf().addAccountAssetTransaction(kUserId, kAssetId, hash, kHeight, i);
      ++txs_added_;
    }
  }

  void prepareState(size_t n) {
    SCOPED_TRACE("prepareState");
    getItf().createDomain(kSecondDomain);
    assertResultValue(getItf().createUserWithPerms(
        kUser, kDomain, kUserKeypair.publicKey(), {Role::kReceive}));
    addAssetTxs(n);
  }

  /**
   * Check the page response.
   * @param response the response of GetAccountAssetTransactions query
   * @param requested_page_start requested first asset (according to the order
   * of addition)
   * @param page_size requested page size
   */
  void validatePageResponse(const TransactionsPageResponse &response,
                            boost::optional<size_t> requested_page_start,
                            size_t page_size) {
    size_t page_start = requested_page_start.value_or(0);
    ASSERT_LE(page_start, txs_added_) << "Bad test.";
    const bool is_last_page = page_start + page_size >= txs_added_;
    const size_t expected_page_size =
        is_last_page ? txs_added_ - page_start : page_size;
    EXPECT_EQ(response.transactions().size(), expected_page_size);
    EXPECT_EQ(response.allTransactionsSize(), txs_added_);
    if (is_last_page) {
      EXPECT_FALSE(response.nextTxHash());
    } else {
      if (not response.nextTxHash()) {
        ADD_FAILURE() << "nextAssetId not set!";
      } else {
        EXPECT_EQ(*response.nextTxHash(),
                  this->makeTxHash(page_start + page_size));
      }
    }
    for (size_t i = 0; i < response.transactions().size(); ++i) {
      EXPECT_EQ(response.transactions()[i].hash(),
                this->makeTxHash(page_start + i));
    }
  }

  void validatePageResponse(const QueryExecutorResult &response,
                            boost::optional<size_t> page_start,
                            size_t page_size) {
    checkSuccessfulResult<TransactionsPageResponse>(
        response, [&, this](const auto &response) {
          this->validatePageResponse(response, page_start, page_size);
        });
  }

  std::unique_ptr<shared_model::interface::MockTxPaginationMeta>
  makePaginationMeta(TransactionsNumberType page_size,
                     boost::optional<HashType> first_tx_hash) {
    return getItf().getMockQueryFactory()->constructTxPaginationMeta(
        page_size, std::move(first_tx_hash));
  }

  /**
   * Query account assets.
   */
  QueryExecutorResult queryPage(boost::optional<size_t> page_start,
                                size_t page_size,
                                AccountIdType command_issuer = kAdminId) {
    boost::optional<HashType> first_tx_hash;
    if (page_start) {
      first_tx_hash = makeTxHash(page_start.value());
    }
    auto pagination_meta = makePaginationMeta(page_size, first_tx_hash);
    return getItf().executeQuery(
        *getItf().getMockQueryFactory()->constructGetAccountAssetTransactions(
            kUserId, kAssetId, *pagination_meta),
        command_issuer);
  }

  /**
   * Query account assets and validate the response.
   */
  QueryExecutorResult queryPageAndValidateResponse(
      boost::optional<size_t> page_start, size_t page_size) {
    auto response = queryPage(page_start, page_size);
    validatePageResponse(response, page_start, page_size);
    return response;
  }

  /// The number of assets added to the default account.
  size_t txs_added_{0};
};

using GetAccountAssetTransactionsBasicTest =
    BasicExecutorTest<GetAccountAssetTransactionsTest>;

/**
 * @given two users with all related permissions
 * @when GetAccountAssetTransactions is queried with nonexistent asset
 * @then there is an empty TransactionsPageResponse
 */
TEST_P(GetAccountAssetTransactionsBasicTest, NonexistentAsset) {
  assertResultValue(getItf().createUserWithPerms(
      kUser, kDomain, kUserKeypair.publicKey(), {}));

  checkQueryError<shared_model::interface::StatefulFailedErrorResponse>(
      queryPage(boost::none, 10), 6);
}

/**
 * @given a user with all related permissions
 * @when GetAccountAssetTransactions is queried on a nonexistent user
 * @then there is an error
 */
TEST_P(GetAccountAssetTransactionsBasicTest, InvalidNoAccount) {
  checkQueryError<shared_model::interface::StatefulFailedErrorResponse>(
      queryPage(boost::none, 10), 5);
}

/**
 * @given two accounts with all related permissions and 10 asset txs
 * @when queried asset txs first page of size 5
 * @then first 5 asset values are returned and are valid
 */
TEST_P(GetAccountAssetTransactionsBasicTest, FirstPage) {
  ASSERT_NO_FATAL_FAILURE(prepareState(10));
  queryPageAndValidateResponse(boost::none, 5);
}

/**
 * @given account with all related permissions and 10 assets
 * @when queried assets page of size 5 starting from 3rd asset
 * @then assets' #3 to #7 values are returned and are valid
 */
TEST_P(GetAccountAssetTransactionsBasicTest, MiddlePage) {
  ASSERT_NO_FATAL_FAILURE(prepareState(10));
  queryPageAndValidateResponse(3, 5);
}

/**
 * @given account with all related permissions and 10 assets
 * @when queried assets page of size 5 starting from 5th asset
 * @then assets' #5 to #9 values are returned and are valid
 */
TEST_P(GetAccountAssetTransactionsBasicTest, LastPage) {
  ASSERT_NO_FATAL_FAILURE(prepareState(10));
  queryPageAndValidateResponse(5, 5);
}

/**
 * @given account with all related permissions and 10 assets
 * @when queried assets page of size 5 starting from 8th asset
 * @then assets' #8 to #9 values are returned and are valid
 */
TEST_P(GetAccountAssetTransactionsBasicTest, PastLastPage) {
  ASSERT_NO_FATAL_FAILURE(prepareState(10));
  queryPageAndValidateResponse(8, 5);
}

/**
 * @given account with all related permissions and 10 assets
 * @when queried assets page of size 5 starting from unknown asset
 * @then error response is returned
 */
TEST_P(GetAccountAssetTransactionsBasicTest, NonexistentStartTx) {
  ASSERT_NO_FATAL_FAILURE(prepareState(10));
  auto response = queryPage(10, 5);
  checkQueryError<shared_model::interface::StatefulFailedErrorResponse>(
      response, error_codes::kInvalidPagination);
}

INSTANTIATE_TEST_CASE_P(Base,
                        GetAccountAssetTransactionsBasicTest,
                        executor_testing::getExecutorTestParams(),
                        executor_testing::paramToString);

using GetAccountAssetTransactionsPermissionTest =
    query_permission_test::QueryPermissionTest<GetAccountAssetTransactionsTest>;

TEST_P(GetAccountAssetTransactionsPermissionTest, QueryPermissionTest) {
  ASSERT_NO_FATAL_FAILURE(prepareState({Role::kReceive}));
  ASSERT_NO_FATAL_FAILURE(addAssetTxs(2));
  auto pagination_meta = makePaginationMeta(txs_added_, boost::none);
  checkResponse<shared_model::interface::TransactionsPageResponse>(
      queryPage(boost::none, txs_added_, getSpectator()),
      [this](
          const shared_model::interface::TransactionsPageResponse &response) {
        this->validatePageResponse(response, boost::none, txs_added_);
      });
}

INSTANTIATE_TEST_CASE_P(
    Common,
    GetAccountAssetTransactionsPermissionTest,
    query_permission_test::getParams({Role::kGetMyAccAstTxs},
                                     {Role::kGetDomainAccAstTxs},
                                     {Role::kGetAllAccAstTxs}),
    query_permission_test::paramToString);
