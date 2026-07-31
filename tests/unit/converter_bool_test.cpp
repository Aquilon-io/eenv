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
#include "eenv/errors.hpp"

namespace {

struct BoolConversionCase {
    std::string raw;
    bool expected;
};

} // namespace

using namespace eenv;

class BoolConverterValidInputs : public ::testing::TestWithParam<BoolConversionCase> {};

TEST_P(BoolConverterValidInputs, ParsesExpectedValue) {
    const auto &[raw, expected] = GetParam();
    EXPECT_EQ(Converter<bool>::convert("field", raw), expected);
}

INSTANTIATE_TEST_SUITE_P(AcceptedValues, BoolConverterValidInputs,
                         ::testing::Values(BoolConversionCase{"true", true}, BoolConversionCase{"1", true},
                                           BoolConversionCase{"false", false}, BoolConversionCase{"0", false},
                                           BoolConversionCase{"True", true}, BoolConversionCase{"TRUE", true},
                                           BoolConversionCase{"yes", true}, BoolConversionCase{"FALSE", false},
                                           BoolConversionCase{"no", false}));

class BoolConverterInvalidInputs : public ::testing::TestWithParam<std::string> {};

TEST_P(BoolConverterInvalidInputs, RejectsValue) {
    EXPECT_THROW(Converter<bool>::convert("field", GetParam()), ConversionError);
}

INSTANTIATE_TEST_SUITE_P(RejectedVariants, BoolConverterInvalidInputs,
                         ::testing::Values(std::string(""), std::string("2"), std::string("-1"), std::string("true "),
                                           std::string(" true"), std::string("truee")));
