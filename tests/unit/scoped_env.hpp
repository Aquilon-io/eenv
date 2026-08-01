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

} // namespace
