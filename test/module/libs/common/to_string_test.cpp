/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "common/to_string.hpp"

#include <memory>
#include <string>
#include <vector>

#include <gmock/gmock-actions.h>
#include <gmock/gmock-matchers.h>
#include <gmock/gmock-spec-builders.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <boost/optional.hpp>
#include <boost/range/any_range.hpp>

using namespace iroha::to_string;
using namespace testing;

const std::string kTestString("test");

struct MockToStringable {
  MOCK_CONST_METHOD0(toString, std::string());
};

std::unique_ptr<MockToStringable> makeObj(std::string string = kTestString) {
  auto obj = std::make_unique<MockToStringable>();
  ON_CALL(*obj, toString()).WillByDefault(Return(string));
  EXPECT_CALL(*obj, toString()).Times(1);
  return obj;
}

const auto kToString = [](auto &&matcher) {
  return ResultOf([](const auto &o) { return toString(o); },
                  std::move(matcher));
};

/**
 * @given std::string
 * @when toString is called on it
 * @then result equals argument
 */
TEST(ToStringTest, StdString) {
  const std::string string("Wake up, Neo...");
  EXPECT_THAT(string, kToString(string));
}

/**
 * @given several plain types that std::to_string accepts
 * @when toString is called on them
 * @then they are converted as std::to_string does
 */
TEST(ToStringTest, PlainValues) {
  EXPECT_THAT(404, kToString("404"));
  EXPECT_THAT(-273, kToString("-273"));
  EXPECT_THAT(15.7f, kToString("15.7"));
  EXPECT_THAT(true, kToString("true"));
}

/**
 * @given ToStringable object
 * @when toString is called on it
 * @then result equals expected string
 */
TEST(ToStringTest, ToStringMethod) {
  EXPECT_THAT(*makeObj(), kToString(kTestString));
}

/**
 * @given ToStringable object wrapped in pointers and optionals
 * @when toString is called on it
 * @then result equals expected string
 */
TEST(ToStringTest, WrappedDereferenceable) {
  // start with unique_ptr
  std::unique_ptr<MockToStringable> o1 = makeObj();
  MockToStringable *raw_obj = o1.get();
  EXPECT_THAT(o1, kToString(kTestString));
  // wrap it into optional
  auto o2 = std::make_optional(std::move(o1));
  EXPECT_CALL(*raw_obj, toString()).Times(1);
  EXPECT_THAT(o2, kToString(kTestString));
  // wrap it into shared_ptr
  auto o3 = std::make_shared<decltype(o2)>(std::move(o2));
  EXPECT_CALL(*raw_obj, toString()).Times(1);
  EXPECT_THAT(o3, kToString(kTestString));
  // wrap it into one more optional
  auto o4 = boost::make_optional(std::move(o3));
  EXPECT_CALL(*raw_obj, toString()).Times(1);
  // boost::optional OEM stream output operator adds a whilespace
  EXPECT_THAT(o4, kToString(" " + kTestString));
}

/**
 * @given unset pointers and optional
 * @when toString is called on them
 * @then result has "not set"
 */
TEST(ToStringTest, UnsetDereferenceable) {
  auto test = [](const auto &o) {
    EXPECT_THAT(o, kToString(AnyOf(Eq("(not set)"), Eq("--"))));
  };
  test(std::unique_ptr<int>{});
  test(std::shared_ptr<int>{});
  test(boost::optional<int>());
  test(boost::none);
  test(std::optional<int>());
  test(std::nullopt);
}

/**
 * @given vector of unique_ptr<ToStringable> objects
 * @when toString is called on it
 * @then result equals expected string
 */
TEST(ToStringTest, VectorOfUniquePointers) {
  std::vector<std::unique_ptr<MockToStringable>> vec;
  EXPECT_THAT(vec, kToString("[]"));
  vec.push_back(makeObj("el1"));
  vec.push_back(makeObj("el2"));
  vec.push_back(nullptr);
  EXPECT_THAT(vec, kToString("[el1, el2, (not set)]"));
}

/**
 * @given vector of unique_ptr<ToStringable> objects
 * @when toString is called on it
 * @then result equals expected string
 */
TEST(ToStringTest, BoostAnyRangeOfSharedPointers) {
  std::vector<std::shared_ptr<MockToStringable>> vec;
  vec.push_back(makeObj("el1"));
  vec.push_back(makeObj("el2"));
  vec.push_back(nullptr);
  boost::any_range<std::shared_ptr<MockToStringable>,
                   boost::forward_traversal_tag,
                   const std::shared_ptr<MockToStringable> &>
      range;
  EXPECT_THAT(range, kToString("[]"));
  range = vec;
  EXPECT_THAT(range, kToString("[el1, el2, (not set)]"));
}
