/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_ROUND_HPP
#define IROHA_ROUND_HPP

#include <cstddef>
#include <cstdint>
#include <string>

#include <boost/operators.hpp>

namespace iroha {
  namespace consensus {

    /// Type of round indexing.
    using Round = uint64_t;

    enum class OutcomeType { kCommit, kReject, kNothing };

    /// Consensus agreement common data.
    struct ConsensusOutcome : public boost::less_than_comparable<Round> {
      using HeightType = shared_model::interface::types::HeightType;

      HeightType height;
      RoundType round;
      OutcomeType type;

      CollaborationOutcome() = default;

      CollaborationOutcome(HeightType height, RoundType round);

      std::string toString() const;
    };

  }  // namespace consensus
}  // namespace iroha

#endif  // IROHA_ROUND_HPP
