//
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

#include <limits>
#include <bit>
#include <cmath>

#include "eenv/constraints.hpp"

using eenv::Range;

using FloatRange_1 = eenv::Range<float, 0.1f, 0.2f>;
using FloatRange_2 = eenv::Range<float, -1.0f, 1.0f>;

TEST(RangeFloat, BoundariesAccepted) {
    EXPECT_NO_THROW(FloatRange_1(0.1f));
    EXPECT_NO_THROW(FloatRange_1(0.2f));
}

TEST(RangeFloat, OneUlpOutsideRejected) {
EXPECT_THROW(
    FloatRange_1(std::nextafterf(0.1f, -std::numeric_limits<float>::infinity())),
    eenv::ConversionError);

EXPECT_THROW(
    FloatRange_1(std::nextafterf(0.2f, std::numeric_limits<float>::infinity())),
    eenv::ConversionError);
}

TEST(RangeFloat, NaNRejected) {
    EXPECT_THROW(FloatRange_2(std::numeric_limits<float>::quiet_NaN()), eenv::ConversionError);
}

TEST(RangeFloat, NegativeZeroAcceptedAsZero) {
    EXPECT_NO_THROW(FloatRange_2(-0.0f));
    EXPECT_NO_THROW(FloatRange_1(0.2f));
}

using FloatRange_Unbounded = eenv::Range<float, 0.0f, std::numeric_limits<float>::infinity()>;

TEST(RangeFloat, InfinityAsUnboundedMax) {
    EXPECT_NO_THROW(FloatRange_Unbounded(std::numeric_limits<float>::infinity()));
}
