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

#include "eenv/constraints.hpp"

using eenv::Range;

using IntRange_1 = eenv::Range<int, 0, 10>;
using IntRange_2 = eenv::Range<int, -1, 1>;

TEST(RangeInt, BoundariesAccepted) {
    EXPECT_NO_THROW(IntRange_1{0});
    EXPECT_NO_THROW(IntRange_1{10});
    EXPECT_NO_THROW(IntRange_2{-1});
    EXPECT_NO_THROW(IntRange_2{0}); 
    EXPECT_NO_THROW(IntRange_2{1}); 
    EXPECT_THROW(IntRange_2{2}, eenv::ConversionError);
    EXPECT_THROW(IntRange_2{-2}, eenv::ConversionError);
}


TEST(RangeInt, NegativeZeroAcceptedAsZero) {
    EXPECT_NO_THROW(IntRange_1{-0});
}

// Compile-time: bool must be rejected
// static_assert(!requires { typename eenv::Range<bool, false, true>; });

