/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_INTERFACE_MOCKS_HPP
#define IROHA_SHARED_MODEL_INTERFACE_MOCKS_HPP

#include <gmock/gmock.h>
#include "cryptography/public_key.hpp"
#include "cryptography/signed.hpp"
#include "interfaces/commands/command.hpp"
#include "interfaces/common_objects/common_objects_factory.hpp"
#include "interfaces/common_objects/peer.hpp"
#include "interfaces/iroha_internal/block.hpp"
#include "interfaces/iroha_internal/block_json_converter.hpp"
#include "interfaces/iroha_internal/proposal.hpp"
#include "interfaces/iroha_internal/transaction_batch.hpp"
#include "interfaces/iroha_internal/unsafe_proposal_factory.hpp"
#include "interfaces/transaction.hpp"

// TODO: 2019-01-18 @muratovv Separate file by classes IR-229
struct MockBlock : public shared_model::Block {
  MOCK_CONST_METHOD0(txsNumber, shared_model::types::TransactionsNumberType());
  MOCK_CONST_METHOD0(transactions,
                     shared_model::types::TransactionsCollectionType());
  MOCK_CONST_METHOD0(rejected_transactions_hashes,
                     shared_model::types::HashCollectionType());
  MOCK_CONST_METHOD0(height, shared_model::types::HeightType());
  MOCK_CONST_METHOD0(prevHash, const shared_model::types::HashType &());
  MOCK_CONST_METHOD0(signatures, shared_model::types::SignatureRangeType());
  MOCK_CONST_METHOD0(createdTime, shared_model::types::TimestampType());
  MOCK_CONST_METHOD0(payload, const shared_model::types::BlobType &());
  MOCK_CONST_METHOD0(blob, const shared_model::types::BlobType &());
  MOCK_METHOD2(addSignature,
               bool(const shared_model::crypto::Signed &,
                    const shared_model::crypto::PublicKey &));
  MOCK_CONST_METHOD0(hash, const shared_model::types::HashType &());
  MOCK_CONST_METHOD0(clone, MockBlock *());
};

struct MockTransaction : public shared_model::Transaction {
  MOCK_CONST_METHOD0(creatorAccountId,
                     const shared_model::types::AccountIdType &());
  MOCK_CONST_METHOD0(quorum, shared_model::types::QuorumType());
  MOCK_CONST_METHOD0(commands, CommandsType());
  MOCK_CONST_METHOD0(reducedHash, const shared_model::types::HashType &());
  MOCK_CONST_METHOD0(hash, const shared_model::types::HashType &());
  MOCK_CONST_METHOD0(
      batch_meta, boost::optional<std::shared_ptr<shared_model::BatchMeta>>());
  MOCK_CONST_METHOD0(signatures, shared_model::types::SignatureRangeType());
  MOCK_CONST_METHOD0(createdTime, shared_model::types::TimestampType());
  MOCK_CONST_METHOD0(payload, const shared_model::types::BlobType &());
  MOCK_CONST_METHOD0(blob, const shared_model::types::BlobType &());
  MOCK_METHOD2(addSignature,
               bool(const shared_model::crypto::Signed &,
                    const shared_model::crypto::PublicKey &));
  MOCK_CONST_METHOD0(clone, MockTransaction *());
  MOCK_CONST_METHOD0(reducedPayload, const shared_model::types::BlobType &());
  MOCK_CONST_METHOD0(
      batchMeta, boost::optional<std::shared_ptr<shared_model::BatchMeta>>());
};

/**
 * Creates mock transaction with provided hash
 * @param hash -- const ref to hash to be returned by the transaction
 * @return shared_ptr for transaction
 */
auto createMockTransactionWithHash(const shared_model::types::HashType &hash) {
  using ::testing::NiceMock;
  using ::testing::ReturnRefOfCopy;

  auto res = std::make_shared<NiceMock<MockTransaction>>();

  ON_CALL(*res, hash()).WillByDefault(ReturnRefOfCopy(hash));

  return res;
}

struct MockTransactionBatch : public shared_model::TransactionBatch {
  MOCK_CONST_METHOD0(transactions,
                     const shared_model::types::SharedTxsCollectionType &());
  MOCK_CONST_METHOD0(reducedHash, const shared_model::types::HashType &());
  MOCK_CONST_METHOD0(hasAllSignatures, bool());
  MOCK_METHOD3(addSignature,
               bool(size_t,
                    const shared_model::crypto::Signed &,
                    const shared_model::crypto::PublicKey &));
  MOCK_CONST_METHOD1(Equals, bool(const shared_model::TransactionBatch &));
  virtual bool operator==(
      const shared_model::TransactionBatch &rhs) const override {
    return Equals(rhs);
  }
  MOCK_CONST_METHOD1(NotEquals, bool(const shared_model::TransactionBatch &));
  MOCK_CONST_METHOD0(clone, MockTransactionBatch *());
};

namespace shared_model {
  std::ostream &operator<<(std::ostream &os, const TransactionBatch &resp) {
    return os << resp.toString();
  }
}  // namespace shared_model

/**
 * Creates mock batch with provided hash
 * @param hash -- const ref to reduced hash to be returned by the batch
 * @return shared_ptr for batch
 */
auto createMockBatchWithHash(const shared_model::types::HashType &hash) {
  using ::testing::NiceMock;
  using ::testing::ReturnRefOfCopy;

  auto res = std::make_shared<NiceMock<MockTransactionBatch>>();

  ON_CALL(*res, reducedHash()).WillByDefault(ReturnRefOfCopy(hash));

  return res;
}

/**
 * Creates mock batch with provided transactions
 * @param txs -- list of transactions in the batch
 * @param hash -- const ref to hash to be returned by the batch
 * @return shared_ptr for batch
 */
auto createMockBatchWithTransactions(
    const shared_model::types::SharedTxsCollectionType &txs, std::string hash) {
  using ::testing::NiceMock;
  using ::testing::ReturnRefOfCopy;

  auto res = std::make_shared<NiceMock<MockTransactionBatch>>();

  ON_CALL(*res, transactions()).WillByDefault(ReturnRefOfCopy(txs));

  ON_CALL(*res, reducedHash())
      .WillByDefault(ReturnRefOfCopy(shared_model::crypto::Hash{hash}));

  return res;
}

struct MockSignature : public shared_model::Signature {
  MOCK_CONST_METHOD0(publicKey, const PublicKeyType &());
  MOCK_CONST_METHOD0(signedData, const SignedType &());
  MOCK_CONST_METHOD0(clone, MockSignature *());
};

struct MockProposal : public shared_model::Proposal {
  MOCK_CONST_METHOD0(transactions,
                     shared_model::types::TransactionsCollectionType());
  MOCK_CONST_METHOD0(height, shared_model::types::HeightType());
  MOCK_CONST_METHOD0(createdTime, shared_model::types::TimestampType());
  MOCK_CONST_METHOD0(blob, const shared_model::types::BlobType &());
  MOCK_CONST_METHOD0(hash, const shared_model::types::HashType &());
  MOCK_CONST_METHOD0(clone, MockProposal *());
};

struct MockPeer : public shared_model::Peer {
  MOCK_CONST_METHOD0(address, const shared_model::types::AddressType &());
  MOCK_CONST_METHOD0(pubkey, const shared_model::types::PubkeyType &());
  MOCK_CONST_METHOD0(
      tlsCertificate,
      const boost::optional<shared_model::types::TLSCertificateType> &());
  MOCK_CONST_METHOD0(clone, MockPeer *());
};

inline auto makePeer(
    const std::string &address,
    const shared_model::crypto::PublicKey &pub_key,
    const boost::optional<shared_model::types::TLSCertificateType>
        &tls_certificate = boost::none) {
  auto peer = std::make_unique<MockPeer>();
  EXPECT_CALL(*peer, address())
      .WillRepeatedly(testing::ReturnRefOfCopy(address));
  EXPECT_CALL(*peer, pubkey())
      .WillRepeatedly(testing::ReturnRefOfCopy(pub_key));
  EXPECT_CALL(*peer, tlsCertificate())
      .WillRepeatedly(testing::ReturnRefOfCopy(tls_certificate));
  return peer;
}

struct MockUnsafeProposalFactory : public shared_model::UnsafeProposalFactory {
  MOCK_METHOD3(unsafeCreateProposal,
               std::unique_ptr<shared_model::Proposal>(
                   shared_model::types::HeightType,
                   shared_model::types::TimestampType,
                   TransactionsCollectionType));
};

struct MockCommonObjectsFactory : public shared_model::CommonObjectsFactory {
  MOCK_METHOD3(
      createPeer,
      FactoryResult<std::unique_ptr<shared_model::Peer>>(
          const shared_model::types::AddressType &,
          const shared_model::types::PubkeyType &,
          const boost::optional<shared_model::types::TLSCertificateType> &));

  MOCK_METHOD4(createAccount,
               FactoryResult<std::unique_ptr<shared_model::Account>>(
                   const shared_model::types::AccountIdType &,
                   const shared_model::types::DomainIdType &,
                   shared_model::types::QuorumType,
                   const shared_model::types::JsonType &));

  MOCK_METHOD3(createAccountAsset,
               FactoryResult<std::unique_ptr<shared_model::AccountAsset>>(
                   const shared_model::types::AccountIdType &,
                   const shared_model::types::AssetIdType &,
                   const shared_model::Amount &));

  MOCK_METHOD3(createAsset,
               FactoryResult<std::unique_ptr<shared_model::Asset>>(
                   const shared_model::types::AssetIdType &,
                   const shared_model::types::DomainIdType &,
                   shared_model::types::PrecisionType));

  MOCK_METHOD2(createDomain,
               FactoryResult<std::unique_ptr<shared_model::Domain>>(
                   const shared_model::types::DomainIdType &,
                   const shared_model::types::RoleIdType &));

  MOCK_METHOD2(createSignature,
               FactoryResult<std::unique_ptr<shared_model::Signature>>(
                   const shared_model::types::PubkeyType &,
                   const shared_model::Signature::SignedType &));
};

struct MockDomain : public shared_model::Domain {
  MOCK_CONST_METHOD0(domainId, shared_model::types::DomainIdType &());
  MOCK_CONST_METHOD0(defaultRole, shared_model::types::RoleIdType &());
  MOCK_CONST_METHOD0(clone, MockDomain *());
};

struct MockAccount : public shared_model::Account {
  MOCK_CONST_METHOD0(accountId, shared_model::types::AccountIdType &());
  MOCK_CONST_METHOD0(domainId, shared_model::types::DomainIdType &());
  MOCK_CONST_METHOD0(quorum, shared_model::types::QuorumType());
  MOCK_CONST_METHOD0(jsonData, shared_model::types::JsonType &());
  MOCK_CONST_METHOD0(clone, MockAccount *());
};

struct MockBlockJsonConverter : public shared_model::BlockJsonConverter {
  MOCK_CONST_METHOD1(
      serialize,
      iroha::expected::Result<shared_model::types::JsonType, std::string>(
          const shared_model::Block &block));
  MOCK_CONST_METHOD1(
      deserialize,
      iroha::expected::Result<std::unique_ptr<shared_model::Block>,
                              std::string>(
          const shared_model::types::JsonType &json));
};

#endif  // IROHA_SHARED_MODEL_INTERFACE_MOCKS_HPP
