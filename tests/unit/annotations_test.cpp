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

#include <cstdlib>
#include <optional>
#include <string>
#include <string_view>

#include "eenv/annotations.hpp"
#include "eenv/errors.hpp"
#include "eenv/settings.hpp"

namespace {

// ----------------------------------------------------------------
// RAII fixture : sets an environment variable for the duration
// of the test and restores the previous state upon destruction.
// Prevents state leakage between tests.
// ----------------------------------------------------------------
class ScopedEnvVar {
  public:
    ScopedEnvVar(std::string name, std::string_view value) : name_(std::move(name)) {
        if (const char *existing = std::getenv(name_.c_str())) {
            previous_ = existing;
        }
        setenv(name_.c_str(), std::string(value).c_str(), /*overwrite=*/1);
    }

    ~ScopedEnvVar() {
        if (previous_) {
            setenv(name_.c_str(), previous_->c_str(), 1);
        } else {
            unsetenv(name_.c_str());
        }
    }

    ScopedEnvVar(const ScopedEnvVar &) = delete;
    ScopedEnvVar &operator=(const ScopedEnvVar &) = delete;

  private:
    std::string name_;
    std::optional<std::string> previous_;
};

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

TEST(AnnotationsRequiredFields, AllMissingFieldsAreAggregatedInOneError) {
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