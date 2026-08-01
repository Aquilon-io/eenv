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

#include <bit>
#include <cmath>
#include <limits>

#include "eenv/constraints.hpp"

using eenv::Range;

using DoubleRange_1 = eenv::Range<double, 0.0, 10.0>;
using DoubleRange_2 = eenv::Range<double, -1.0, 1.0>;

TEST(RangeDouble, BoundariesAccepted) {
    EXPECT_NO_THROW(DoubleRange_1(0.0));
    EXPECT_NO_THROW(DoubleRange_1(10.0));
    EXPECT_NO_THROW(DoubleRange_2(-1.0));
    EXPECT_NO_THROW(DoubleRange_2(0.0));
    EXPECT_NO_THROW(DoubleRange_2(1.0));
}

TEST(RangeDouble, OneUlpOutsideRejected) {
    EXPECT_THROW(DoubleRange_1(std::nextafter(0.0, -HUGE_VAL)), eenv::ConversionError);
    EXPECT_THROW(DoubleRange_1(std::nextafter(10.0, +HUGE_VAL)), eenv::ConversionError);
}

TEST(RangeDouble, NaNRejected) {
    EXPECT_THROW(DoubleRange_1(std::numeric_limits<double>::quiet_NaN()), eenv::ConversionError);
}

TEST(RangeDouble, NegativeZeroAcceptedAsZero) { EXPECT_NO_THROW(DoubleRange_1(-0.0)); }

using DoubleRange_Unbounded = eenv::Range<double, 0.0, std::numeric_limits<double>::infinity()>;

TEST(RangeDouble, InfinityAsUnboundedMax) {
    EXPECT_NO_THROW(DoubleRange_Unbounded(std::numeric_limits<double>::infinity()));
    EXPECT_NO_THROW(DoubleRange_Unbounded(1e300));
}
