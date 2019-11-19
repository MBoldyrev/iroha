/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_SHARED_MODEL_SET_SETTING_VALUE_HPP
#define IROHA_SHARED_MODEL_SET_SETTING_VALUE_HPP

#include "interfaces/base/model_primitive.hpp"

#include "interfaces/common_objects/types.hpp"

namespace shared_model {

  /**
   * Set key-value pair of settings
   */
  class SetSettingValue : public ModelPrimitive<SetSettingValue> {
   public:
    /**
     * @return key of data to store in settings
     */
    const types::SettingKeyType &key() const;

    /**
     * @return setting value to store by given key
     */
    const types::SettingValueType &value() const;

    std::string toString() const override;

    bool operator==(const ModelType &rhs) const override;
  };
}  // namespace shared_model

#endif  // IROHA_SHARED_MODEL_SET_SETTING_VALUE_HPP
