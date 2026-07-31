// Copyright (c) 2026 Sylvain Ladoux
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <gtest/gtest.h>

#include "eenv/constraints.hpp"

using namespace eenv;


// TEST(RangeBounds, ExactBoundariesAccepted) {
//     EXPECT_TRUE(in_range<Range<double, 0.1, 0.2>>(0.1));
//     EXPECT_TRUE(in_range<Range<double, 0.1, 0.2>>(0.2));
// }

// TEST(RangeBounds, OneUlpOutsideRejected) {
//     constexpr double min = 0.1, max = 0.2;
//     EXPECT_FALSE(in_range<Range<double, min, max>>(std::nextafter(min, -HUGE_VAL)));
//     EXPECT_FALSE(in_range<Range<double, min, max>>(std::nextafter(max, +HUGE_VAL)));
// }

// TEST(RangeBounds, NaNAlwaysRejected) {
//     EXPECT_FALSE(in_range<Range<double, 0.0, 1.0>>(
//         std::numeric_limits<double>::quiet_NaN()));
// }

// TEST(RangeBounds, NegativeZeroTreatedAsZero) {
//     EXPECT_TRUE(in_range<Range<double, 0.0, 1.0>>(-0.0));
// }

// TEST(RangeBounds, StringParsingBitIdentical) {
//     double parsed;
//     std::from_chars("0.1", "0.1" + 3, parsed);
//     EXPECT_EQ(std::bit_cast<std::uint64_t>(parsed),
//               std::bit_cast<std::uint64_t>(0.1));
// }

