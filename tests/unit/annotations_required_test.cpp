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

#include "scoped_env.hpp"

#include "eenv/annotations.hpp"
#include "eenv/settings.hpp"

namespace {

// clang-format off

struct OneRequiredField {
    [[= env::required]] 
    int port;
};

struct TwoRequiredFields {
    [[= env::required]] 
    int port;
    [[= env::required]] 
    std::string host;
};

struct [[= env::required]] AllRequiredFields {
    int port;
    std::string host;
};

struct [[= env::required]] AllRequiredFieldsUnlessOptional {
    [[= env::optional]]
    int port;
    std::string host;
};

// clang-format on

} // namespace

using namespace eenv;

TEST(AnnotationsRequiredFields, MissingRequiredFieldThrows) {
    unsetenv("PORT");
    EXPECT_THROW(from_env<OneRequiredField>(), SettingsError);
}

TEST(AnnotationsRequiredFields, PresentRequiredFieldSucceeds) {
    ScopedEnvVar guard("PORT", "8080");
    const auto settings = from_env<OneRequiredField>();
    EXPECT_EQ(settings.port, 8080);
}

TEST(AnnotationsRequiredFields, TwoRequiredFieldsAggregatedInOneError) {
    unsetenv("PORT");
    unsetenv("HOST");

    try {
        from_env<TwoRequiredFields>();
        FAIL() << "from_env should have raised a SettingsError";
    } catch (const SettingsError &e) {
        EXPECT_EQ(e.errors().size(), 2u);
        // EXPECT_TRUE(e.has_error_for("port"));
        // EXPECT_TRUE(e.has_error_for("host"));
    }
}

TEST(AnnotationsRequiredFields, OneMissingOnePresentReportsOnlyTheMissingOne) {
    ScopedEnvVar guard("PORT", "8080");
    unsetenv("HOST");

    try {
        from_env<TwoRequiredFields>();
        FAIL() << "from_env should have raised a SettingsError";
    } catch (const SettingsError &e) {
        EXPECT_EQ(e.errors().size(), 1u);
        // EXPECT_TRUE(e.has_error_for("host"));
        // EXPECT_FALSE(e.has_error_for("port"));
    }
}

TEST(AnnotationsRequiredFields, AllMissingFieldsAggregatedInOneError) {
    unsetenv("PORT");
    unsetenv("HOST");

    try {
        from_env<AllRequiredFields>();
        FAIL() << "from_env should have raised a SettingsError";
    } catch (const SettingsError &e) {
        EXPECT_EQ(e.errors().size(), 2u);
        // EXPECT_TRUE(e.has_error_for("port"));
        // EXPECT_TRUE(e.has_error_for("host"));
    }
}

TEST(AnnotationsRequiredFields, AllRequiredFieldsUnlessOptionalAggregatedInOneError) {
    unsetenv("PORT");
    unsetenv("HOST");

    try {
        from_env<AllRequiredFieldsUnlessOptional>();
        FAIL() << "from_env should have raised a SettingsError";
    } catch (const SettingsError &e) {
        EXPECT_EQ(e.errors().size(), 1u);
        // EXPECT_TRUE(e.has_error_for("port"));
        // EXPECT_TRUE(e.has_error_for("host"));
    }
}

namespace {

struct BadConfig {
    [[= env::required]] std::optional<int> port;
};

} // namespace

// The static_assert in settings.hpp should trigger a compile-time error, so this test is commented out.
// See from_env() in settings.hpp for the note on the static_assert that enforces this rule.
//
// TEST(AnnotationsRequiredFields, WillFail)
// {
//     // This test is expected to fail at compile time due to the static_assert in settings
//     try {
//         from_env<BadConfig>();
//
//     } catch (const SettingsError &e) {
//         // ...
//     }
// }