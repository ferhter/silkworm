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

#include "util.hpp"

namespace silkworm {

std::string_view zeroless_view(const evmc::bytes32& hash) {
  unsigned zero_bytes{0};
  while (zero_bytes < kHashLength && hash.bytes[zero_bytes] == 0) {
    ++zero_bytes;
  }
  return {byte_ptr_cast(hash.bytes + zero_bytes), kHashLength - zero_bytes};
}
}  // namespace silkworm