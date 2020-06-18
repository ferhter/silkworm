/*
   Copyright 2020 The Silkworm Authors

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include "execution.hpp"

#include <algorithm>
#include <gsl/gsl_assert>
#include <intx/int128.hpp>
#include <limits>
#include <string_view>
#include <utility>

#include "protocol_params.hpp"

namespace {

using namespace silkworm::eth;

intx::uint<128> IntrinsicGas(std::string_view data, bool contract_creation, bool homestead,
                             bool eip2028) {
  intx::uint<128> gas = params::kTxGas;
  if (contract_creation && homestead) {
    gas = params::kTxGasContractCreation;
  }

  if (data.empty()) {
    return gas;
  }

  intx::uint<128> non_zero_bytes =
      std::count_if(data.begin(), data.end(), [](char c) { return c != 0; });

  uint64_t nonZeroGas{eip2028 ? params::kTxDataNonZeroGasEIP2028
                              : params::kTxDataNonZeroGasFrontier};
  gas += non_zero_bytes * nonZeroGas;

  intx::uint<128> zero_bytes = data.length() - non_zero_bytes;
  gas += zero_bytes * params::kTxDataZeroGas;

  return gas;
}

}  // namespace

namespace silkworm::eth {

ExecutionResult ExecutionProcessor::ExecuteTransaction(const Transaction& txn) {
  Expects(txn.from);

  ExecutionResult res;

  uint64_t nonce = state_.GetNonce(*txn.from);
  if (nonce < txn.nonce) {
    res.err = ExecutionError::kNonceTooHigh;
    return res;
  } else if (nonce > txn.nonce) {
    res.err = ExecutionError::kNonceTooLow;
    return res;
  }

  bool homestead = chain_config_.IsHomestead(block_number_);
  bool istanbul = chain_config_.IsIstanbul(block_number_);
  bool contract_creation = !txn.to;

  intx::uint<128> gas = IntrinsicGas(txn.data, contract_creation, homestead, istanbul);
  if (txn.gas_limit < gas) {
    res.err = ExecutionError::kIntrinsicGas;
    return res;
  }

  // TODO (Andrew) implement

  return res;
}

}  // namespace silkworm::eth