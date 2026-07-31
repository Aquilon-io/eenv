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
#include <string>
#include <string_view>
#include <type_traits>

#include "eenv/convert.hpp"
#include "eenv/errors.hpp"

using namespace eenv;

// ---------------------------------------------------------------------
// Test adapter
// ---------------------------------------------------------------------
template <typename T> struct ConverterTraits {
    static T convert(std::string_view raw, std::string_view field = "field") {
        return Converter<T>::convert(field, raw);
    }
};

// ---------------------------------------------------------------------
// Typed parameter fixture
// ---------------------------------------------------------------------
template <typename T> class IntegralConverterConformance : public ::testing::Test {};

TYPED_TEST_SUITE_P(IntegralConverterConformance);

TYPED_TEST_P(IntegralConverterConformance, ParsesTypicalValue) {
    using T = TypeParam;
    EXPECT_EQ(ConverterTraits<T>::convert("42"), static_cast<T>(42));
}

TYPED_TEST_P(IntegralConverterConformance, RoundTripsMax) {
    using T = TypeParam;
    const T expected = std::numeric_limits<T>::max();
    EXPECT_EQ(ConverterTraits<T>::convert(std::to_string(expected)), expected);
}

TYPED_TEST_P(IntegralConverterConformance, RoundTripsMin) {
    using T = TypeParam;
    const T expected = std::numeric_limits<T>::min();
    EXPECT_EQ(ConverterTraits<T>::convert(std::to_string(expected)), expected);
}

TYPED_TEST_P(IntegralConverterConformance, RejectsEmptyString) {
    using T = TypeParam;
    EXPECT_THROW(ConverterTraits<T>::convert(""), ConversionError);
}

TYPED_TEST_P(IntegralConverterConformance, RejectsTrailingGarbage) {
    using T = TypeParam;
    EXPECT_THROW(ConverterTraits<T>::convert("42abc"), ConversionError);
}

TYPED_TEST_P(IntegralConverterConformance, RejectsLeadingWhitespace) {
    using T = TypeParam;
    EXPECT_THROW(ConverterTraits<T>::convert(" 42"), ConversionError);
}

TYPED_TEST_P(IntegralConverterConformance, RejectsOverflow) {
    using T = TypeParam;
    // 40 '9' : garantit un dépassement quel que soit T, même sur 128 bits.
    const std::string overflowed(40, '9');
    EXPECT_THROW(ConverterTraits<T>::convert(overflowed), ConversionError);
}

TYPED_TEST_P(IntegralConverterConformance, RejectsNegativeWhenUnsigned) {
    using T = TypeParam;
    if constexpr (std::is_unsigned_v<T>) {
        EXPECT_THROW(ConverterTraits<T>::convert("-1"), ConversionError);
    } else {
        GTEST_SKIP() << "T is signed, -1 is a valid value";
    }
}

REGISTER_TYPED_TEST_SUITE_P(IntegralConverterConformance, ParsesTypicalValue, RoundTripsMax, RoundTripsMin,
                            RejectsEmptyString, RejectsTrailingGarbage, RejectsLeadingWhitespace, RejectsOverflow,
                            RejectsNegativeWhenUnsigned);

// ---------------------------------------------------------------------
// Instanciate typed tests
// ---------------------------------------------------------------------
using SignedIntegralTypes = ::testing::Types<short, int, long, long long>;
using UnsignedIntegralTypes = ::testing::Types<unsigned short, unsigned int, unsigned long, unsigned long long>;

INSTANTIATE_TYPED_TEST_SUITE_P(Signed, IntegralConverterConformance, SignedIntegralTypes);
INSTANTIATE_TYPED_TEST_SUITE_P(Unsigned, IntegralConverterConformance, UnsignedIntegralTypes);
