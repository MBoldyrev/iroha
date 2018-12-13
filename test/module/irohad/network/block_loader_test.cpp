/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <grpc++/security/server_credentials.h>
#include <grpc++/server.h>
#include <grpc++/server_builder.h>
#include <gtest/gtest.h>

#include "builders/protobuf/builder_templates/transaction_template.hpp"
#include "consensus/consensus_block_cache.hpp"
#include "cryptography/crypto_provider/crypto_defaults.hpp"
#include "cryptography/hash.hpp"
#include "datetime/time.hpp"
#include "framework/test_subscriber.hpp"
#include "module/irohad/ametsuchi/ametsuchi_mocks.hpp"
#include "module/shared_model/builders/common_objects/peer_builder.hpp"
#include "module/shared_model/builders/protobuf/common_objects/proto_peer_builder.hpp"
#include "module/shared_model/builders/protobuf/test_block_builder.hpp"
#include "module/shared_model/builders/protobuf/test_transaction_builder.hpp"
#include "network/impl/block_loader_impl.hpp"
#include "network/impl/block_loader_service.hpp"
#include "validators/default_validator.hpp"

using namespace iroha::network;
using namespace iroha::ametsuchi;
using namespace framework::test_subscriber;
using namespace shared_model::crypto;
using namespace shared_model::validation;

using testing::_;
using testing::A;
using testing::Return;

using wPeer = std::shared_ptr<shared_model::interface::Peer>;
using wBlock = std::shared_ptr<shared_model::interface::Block>;

class BlockLoaderTest : public testing::Test {
 public:
  void SetUp() override {
    peer_query = std::make_shared<MockPeerQuery>();
    peer_query_factory = std::make_shared<MockPeerQueryFactory>();
    EXPECT_CALL(*peer_query_factory, createPeerQuery())
        .WillRepeatedly(testing::Return(boost::make_optional(
            std::shared_ptr<iroha::ametsuchi::PeerQuery>(peer_query))));
    storage = std::make_shared<MockBlockQuery>();
    block_query_factory = std::make_shared<MockBlockQueryFactory>();
    EXPECT_CALL(*block_query_factory, createBlockQuery())
        .WillRepeatedly(testing::Return(boost::make_optional(
            std::shared_ptr<iroha::ametsuchi::BlockQuery>(storage))));
    block_cache = std::make_shared<iroha::consensus::ConsensusResultCache>();
    auto validator_ptr =
        std::make_unique<MockValidator<shared_model::interface::Block>>();
    validator = validator_ptr.get();
    loader = std::make_shared<BlockLoaderImpl>(
        peer_query_factory,
        shared_model::proto::ProtoBlockFactory(
            std::move(validator_ptr),
            std::make_unique<MockValidator<iroha::protocol::Block>>()));
    service =
        std::make_shared<BlockLoaderService>(block_query_factory, block_cache);

    grpc::ServerBuilder builder;
    int port = 0;
    builder.AddListeningPort(
        "0.0.0.0:0", grpc::InsecureServerCredentials(), &port);
    builder.RegisterService(service.get());
    server = builder.BuildAndStart();

    shared_model::builder::PeerBuilder<
        shared_model::proto::PeerBuilder,
        shared_model::validation::FieldValidator>()
        .address("0.0.0.0:" + std::to_string(port))
        .pubkey(peer_key)
        .build()
        .match(
            [&](iroha::expected::Value<
                std::shared_ptr<shared_model::interface::Peer>> &v) {
              peer = std::move(v.value);
            },
            [](iroha::expected::Error<std::shared_ptr<std::string>>) {});

    ASSERT_TRUE(server);
    ASSERT_NE(port, 0);
  }

  auto getBaseBlockBuilder() const {
    std::vector<shared_model::proto::Transaction> txs;
    txs.push_back(TestUnsignedTransactionBuilder()
                      .creatorAccountId("account@domain")
                      .setAccountQuorum("account@domain", 1)
                      .createdTime(iroha::time::now())
                      .quorum(1)
                      .build()
                      .signAndAddSignature(key)
                      .finish());
    return shared_model::proto::TemplateBlockBuilder<
               (1 << shared_model::proto::TemplateBlockBuilder<>::total) - 1,
               shared_model::validation::AlwaysValidValidator,
               shared_model::proto::UnsignedWrapper<
                   shared_model::proto::Block>>()
        .height(1)
        .prevHash(kPrevHash)
        .createdTime(iroha::time::now())
        .transactions(txs);
  }

  auto getBaseBlockBuilder(const Hash &prev_hash) const {
    std::vector<shared_model::proto::Transaction> txs;
    txs.push_back(TestUnsignedTransactionBuilder()
                      .creatorAccountId("account@domain")
                      .setAccountQuorum("account@domain", 1)
                      .createdTime(iroha::time::now())
                      .quorum(1)
                      .build()
                      .signAndAddSignature(key)
                      .finish());
    return shared_model::proto::TemplateBlockBuilder<
               (1 << shared_model::proto::TemplateBlockBuilder<>::total) - 1,
               shared_model::validation::AlwaysValidValidator,
               shared_model::proto::UnsignedWrapper<
                   shared_model::proto::Block>>()
        .height(1)
        .prevHash(prev_hash)
        .createdTime(iroha::time::now())
        .transactions(txs);
  }

  const Hash kPrevHash =
      Hash(std::string(DefaultCryptoAlgorithmType::kHashLength, '0'));

  std::shared_ptr<shared_model::interface::Peer> peer;
  PublicKey peer_key =
      DefaultCryptoAlgorithmType::generateKeypair().publicKey();
  Keypair key = DefaultCryptoAlgorithmType::generateKeypair();
  std::shared_ptr<MockPeerQuery> peer_query;
  std::shared_ptr<MockPeerQueryFactory> peer_query_factory;
  std::shared_ptr<MockBlockQuery> storage;
  std::shared_ptr<MockBlockQueryFactory> block_query_factory;
  std::shared_ptr<BlockLoaderImpl> loader;
  std::shared_ptr<BlockLoaderService> service;
  std::unique_ptr<grpc::Server> server;
  std::shared_ptr<iroha::consensus::ConsensusResultCache> block_cache;
  MockValidator<shared_model::interface::Block> *validator;
};

/**
 * Current block height 1 => Other block height 1 => no blocks received
 * @given empty storage, related block loader and base block
 * @when retrieveBlocks is called
 * @then nothing is returned
 */
TEST_F(BlockLoaderTest, ValidWhenSameTopBlock) {
  auto block = getBaseBlockBuilder().build().signAndAddSignature(key).finish();

  EXPECT_CALL(*peer_query, getLedgerPeers())
      .WillOnce(Return(std::vector<wPeer>{peer}));
  EXPECT_CALL(*storage, getBlocksFrom(block.height() + 1))
      .WillOnce(Return(std::vector<wBlock>()));

  auto wrapper = make_test_subscriber<CallExact>(
      loader->retrieveBlocks(1, peer->pubkey()), 0);
  wrapper.subscribe();

  ASSERT_TRUE(wrapper.validate());
}

/**
 * @given block loader and a pair of consecutive blocks
 * @when retrieveBlocks is called
 * @then the last one is returned
 */
TEST_F(BlockLoaderTest, ValidWhenOneBlock) {
  // Current block height 1 => Other block height 2 => one block received
  // time validation should work based on the block field
  // so it should pass stateless BlockLoader validation
  auto block = getBaseBlockBuilder()
                   .createdTime(228)
                   .build()
                   .signAndAddSignature(key)
                   .finish();

  auto top_block = getBaseBlockBuilder()
                       .createdTime(block.createdTime() + 1)
                       .height(block.height() + 1)
                       .build()
                       .signAndAddSignature(key)
                       .finish();

  EXPECT_CALL(*peer_query, getLedgerPeers())
      .WillOnce(Return(std::vector<wPeer>{peer}));
  EXPECT_CALL(*storage, getBlocksFrom(block.height() + 1))
      .WillOnce(Return(std::vector<wBlock>{clone(top_block)}));
  auto wrapper =
      make_test_subscriber<CallExact>(loader->retrieveBlocks(1, peer_key), 1);
  wrapper.subscribe(
      [&top_block](auto block) { ASSERT_EQ(*block.operator->(), top_block); });

  ASSERT_TRUE(wrapper.validate());
}

/**
 * @given block loader, a block, and additional num_blocks blocks
 * @when retrieveBlocks is called
 * @then it returns consecutive heights
 */
TEST_F(BlockLoaderTest, ValidWhenMultipleBlocks) {
  // Current block height 1 => Other block height n => n-1 blocks received
  // time validation should work based on the block field
  // so it should pass stateless BlockLoader validation
  auto block = getBaseBlockBuilder()
                   .createdTime(1337)
                   .build()
                   .signAndAddSignature(key)
                   .finish();

  auto num_blocks = 2;
  auto next_height = block.height() + 1;

  std::vector<wBlock> blocks;
  for (auto i = next_height; i < next_height + num_blocks; ++i) {
    auto blk = getBaseBlockBuilder()
                   .height(i)
                   .build()
                   .signAndAddSignature(key)
                   .finish();
    blocks.emplace_back(clone(blk));
  }

  EXPECT_CALL(*peer_query, getLedgerPeers())
      .WillOnce(Return(std::vector<wPeer>{peer}));
  EXPECT_CALL(*storage, getBlocksFrom(next_height)).WillOnce(Return(blocks));
  auto wrapper = make_test_subscriber<CallExact>(
      loader->retrieveBlocks(1, peer_key), num_blocks);
  auto height = next_height;
  wrapper.subscribe(
      [&height](auto block) { ASSERT_EQ(block->height(), height++); });

  ASSERT_TRUE(wrapper.validate());
}

MATCHER_P(RefAndPointerEq, arg1, "") {
  return arg == *arg1;
}
/**
 * @given block loader @and consensus cache with a block
 * @when retrieveBlock is called with the related hash
 * @then it returns the same block @and block loader service does not ask
 * storage
 */
TEST_F(BlockLoaderTest, ValidWhenBlockPresent) {
  // Request existing block => success
  auto block = std::make_shared<shared_model::proto::Block>(
      getBaseBlockBuilder().build().signAndAddSignature(key).finish());
  block_cache->insert(block);

  EXPECT_CALL(*peer_query, getLedgerPeers())
      .WillOnce(Return(std::vector<wPeer>{peer}));
  EXPECT_CALL(*validator, validate(RefAndPointerEq(block)))
      .WillOnce(Return(Answer{}));
  EXPECT_CALL(*storage, getBlocksFrom(_)).Times(0);
  auto retrieved_block = loader->retrieveBlock(peer_key, block->hash());

  ASSERT_TRUE(retrieved_block);
  ASSERT_EQ(*block, **retrieved_block);
}

/**
 * @given block loader @and consensus cache with a block @and mocked storage
 * with two blocks
 * @when retrieveBlock is called with a hash of previous block
 * @then consensus cache is missed @and block loader tries to fetch block from
 * the storage
 */
TEST_F(BlockLoaderTest, ValidWhenBlockMissing) {
  auto prev_block = std::make_shared<shared_model::proto::Block>(
      getBaseBlockBuilder().build().signAndAddSignature(key).finish());
  auto cur_block = std::make_shared<shared_model::proto::Block>(
      getBaseBlockBuilder(prev_block->hash())
          .build()
          .signAndAddSignature(key)
          .finish());
  block_cache->insert(cur_block);

  EXPECT_CALL(*peer_query, getLedgerPeers())
      .WillOnce(Return(std::vector<wPeer>{peer}));
  EXPECT_CALL(*storage, getBlocksFrom(1))
      .WillOnce(
          Return(std::vector<std::shared_ptr<shared_model::interface::Block>>{
              prev_block, cur_block}));

  auto block = loader->retrieveBlock(peer_key, prev_block->hash());
  ASSERT_TRUE(block);
  ASSERT_EQ(block.value()->hash(), prev_block->hash());
}

/**
 * @given block loader @and empty consensus cache @and two blocks in storage
 * @when retrieveBlock is called with first block's hash
 * @then consensus cache is missed @and block loader tries to fetch block from
 * the storage
 */
TEST_F(BlockLoaderTest, ValidWithEmptyCache) {
  auto prev_block = std::make_shared<shared_model::proto::Block>(
      getBaseBlockBuilder().build().signAndAddSignature(key).finish());
  auto cur_block = std::make_shared<shared_model::proto::Block>(
      getBaseBlockBuilder(prev_block->hash())
          .build()
          .signAndAddSignature(key)
          .finish());

  EXPECT_CALL(*peer_query, getLedgerPeers())
      .WillOnce(Return(std::vector<wPeer>{peer}));
  EXPECT_CALL(*storage, getBlocksFrom(1))
      .WillOnce(
          Return(std::vector<std::shared_ptr<shared_model::interface::Block>>{
              prev_block, cur_block}));

  auto block = loader->retrieveBlock(peer_key, prev_block->hash());
  ASSERT_TRUE(block);
  ASSERT_EQ(block.value()->hash(), prev_block->hash());
}

/**
 * @given block loader @and empty consensus cache @and no blocks in storage
 * @when retrieveBlock is called with some block hash
 * @then consensus cache is missed @and block storage is missed @and block
 * loader returns nothing
 */
TEST_F(BlockLoaderTest, NoBlocksInStorage) {
  EXPECT_CALL(*peer_query, getLedgerPeers())
      .WillOnce(Return(std::vector<wPeer>{peer}));
  EXPECT_CALL(*storage, getBlocksFrom(1))
      .WillOnce(Return(
          std::vector<std::shared_ptr<shared_model::interface::Block>>{}));

  auto block = loader->retrieveBlock(peer_key, kPrevHash);
  ASSERT_FALSE(block);
}
