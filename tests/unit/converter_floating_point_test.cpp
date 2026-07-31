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

struct FloatConversionCase {
    std::string raw;
    float expected;
};

class FloatConverterValidInputs : public ::testing::TestWithParam<FloatConversionCase> {};

TEST_P(FloatConverterValidInputs, ParsesExpectedValue) {
    const auto &[raw, expected] = GetParam();
    EXPECT_EQ(Converter<float>::convert("field", raw), expected);
}

INSTANTIATE_TEST_SUITE_P(AcceptedValues, FloatConverterValidInputs,
                         ::testing::Values(FloatConversionCase{std::string("0.0"), 0.0f},
                                           FloatConversionCase{std::string("1.0"), 1.0f},
                                           FloatConversionCase{std::string("42"), 42.0f},
                                           FloatConversionCase{std::string("-1.0"), -1.0f},
                                           FloatConversionCase{std::string("0.0004"), 0.0004f},
                                           FloatConversionCase{std::string("50000.0"), 50000.0f},
                                           FloatConversionCase{std::string("0.4"), 0.4f},
                                           FloatConversionCase{std::string("-12.0"), -12.0f},
                                           FloatConversionCase{std::string("0.001"), 0.001f} ));

class FloatConverterInvalidInputs : public ::testing::TestWithParam<std::string> {};

TEST_P(FloatConverterInvalidInputs, RejectsValue) {
    EXPECT_THROW(Converter<float>::convert("field", GetParam()), ConversionError);
}

INSTANTIATE_TEST_SUITE_P(RejectedVariants, FloatConverterInvalidInputs,
                         ::testing::Values(std::string(""), std::string("abc"), std::string("-1b"), std::string("true ")
                                        ));

