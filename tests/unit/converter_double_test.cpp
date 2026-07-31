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

#include <string>

#include "eenv/convert.hpp"

using namespace eenv;

struct DoubleConversionCase {
    std::string raw;
    double expected;
};

class DoubleConverterValidInputs : public ::testing::TestWithParam<DoubleConversionCase> {};

TEST_P(DoubleConverterValidInputs, ParsesExpectedValue) {
    const auto &[raw, expected] = GetParam();
    EXPECT_EQ(Converter<double>::convert("field", raw), expected);
}

INSTANTIATE_TEST_SUITE_P(AcceptedValues, DoubleConverterValidInputs,
                         ::testing::Values(DoubleConversionCase{std::string("0.0"), 0.0},
                                           DoubleConversionCase{std::string("1.0000001"), 1.0000001},
                                           DoubleConversionCase{std::string("42"), 42.0},
                                           DoubleConversionCase{std::string("-1.0"), -1.0},
                                           DoubleConversionCase{std::string("0.0004"), 0.0004},
                                           DoubleConversionCase{std::string("50000.0"), 50000.0},
                                           DoubleConversionCase{std::string("0.4"), 0.4},
                                           DoubleConversionCase{std::string("-12.0"), -12.0},
                                           DoubleConversionCase{std::string("0.0099999991"), 0.0099999991} ));

class DoubleConverterInvalidInputs : public ::testing::TestWithParam<std::string> {};

TEST_P(DoubleConverterInvalidInputs, RejectsValue) {
    EXPECT_THROW(Converter<double>::convert("field", GetParam()), ConversionError);
}

INSTANTIATE_TEST_SUITE_P(RejectedVariants, DoubleConverterInvalidInputs,
                         ::testing::Values(std::string(""), std::string("abc"), std::string("-1b"), std::string("true ")
                                        ));

