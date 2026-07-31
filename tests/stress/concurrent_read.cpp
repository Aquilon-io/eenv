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

#include <barrier>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <string>
#include <thread>
#include <vector>

#include "eenv/eenv.hpp"

// This test, when run with ThreadSanitizer (-fsanitize=thread),
// verifies that concurrent calls:
// - do not introduce internal data races;
// - do not cause memory corruption;
// - do not rely on unsafe shared mutable state.

namespace {

constexpr std::size_t kWorkerCount = 32;
constexpr std::size_t kIterationsPerWorker = 2000;

void set_environment(std::string_view name, std::string_view value) {
#ifdef _WIN32
    _putenv_s(name.data(), value.data());
#else
    setenv(name.data(), value.data(), 1);
#endif
}

// Each worker writes only its own outcome.
// Results are read after all workers have joined.
struct Outcome {
    bool ok = true;
    std::string error;
};

struct ExpectedSettings {
    std::string token;
    int retry_count;
    bool feature_enabled;
    double timeout_seconds;
};

} // namespace

int main() {
    const ExpectedSettings expected{
        .token = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9."
                 "eyJzdWIiOiIxMjM0NTY3ODkwIiwi"
                 "bmFtZSI6IkVudlRlc3RVc2VyIiwic2NvcGUiOiJhcGk6cmVhZCJ9."
                 "SflKxwRJSMeKKF2QT4fwpMeJf36POk6yJV_adQssw5c",
        .retry_count = 7,
        .feature_enabled = true,
        .timeout_seconds = 12.5,
    };

    struct[[= env::case_insensitive]] ConcurrentSettings {
        std::string token;
        int retry_count = 0;
        bool feature_enabled = false;
        double timeout_seconds = 0.0;
        std::optional<std::string> optional_tag;
    };

    set_environment("TOKEN", expected.token);
    set_environment("RETRY_COUNT", "7");
    set_environment("FEATURE_ENABLED", "true");
    set_environment("TIMEOUT_SECONDS", "12.5");

    std::vector<Outcome> outcomes(kWorkerCount);
    std::barrier start_barrier(kWorkerCount + 1);

    std::vector<std::thread> workers;
    workers.reserve(kWorkerCount);

    for (std::size_t i = 0; i < kWorkerCount; ++i) {
        workers.emplace_back([&, i] {
            start_barrier.arrive_and_wait();

            for (std::size_t iteration = 0; iteration < kIterationsPerWorker; ++iteration) {
                try {
                    const auto settings = eenv::from_env<ConcurrentSettings>();

                    if (settings.token != expected.token) {
                        outcomes[i] = {false, "invalid token at iteration " + std::to_string(iteration)};
                        return;
                    }

                    if (settings.retry_count != expected.retry_count) {
                        outcomes[i] = {false, "invalid retry_count at iteration " + std::to_string(iteration)};
                        return;
                    }

                    if (settings.feature_enabled != expected.feature_enabled) {
                        outcomes[i] = {false, "invalid feature_enabled at iteration " + std::to_string(iteration)};
                        return;
                    }

                    if (settings.timeout_seconds != expected.timeout_seconds) {
                        outcomes[i] = {false, "invalid timeout_seconds at iteration " + std::to_string(iteration)};
                        return;
                    }

                    if (settings.optional_tag.has_value()) {
                        outcomes[i] = {false, "optional value unexpectedly initialized at iteration " +
                                                  std::to_string(iteration)};
                        return;
                    }

                } catch (const std::exception &e) {
                    outcomes[i] = {false, "exception at iteration " + std::to_string(iteration) + ": " + e.what()};
                    return;
                }
            }
        });
    }

    start_barrier.arrive_and_wait();

    for (auto &worker : workers) {
        worker.join();
    }

    int failures = 0;

    for (std::size_t i = 0; i < kWorkerCount; ++i) {
        if (!outcomes[i].ok) {
            ++failures;
            std::cerr << "Worker " << i << " failed: " << outcomes[i].error << '\n';
        }
    }

    if (failures == 0) {
        std::cout << kWorkerCount << " workers completed " << kIterationsPerWorker
                  << " concurrent from_env<T>() calls each.\n";
        return 0;
    }

    std::cerr << failures << "/" << kWorkerCount << " workers reported failures.\n";

    return 1;
}
