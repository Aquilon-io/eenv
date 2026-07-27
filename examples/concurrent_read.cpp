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

#include <cstddef>
#include <iostream>
#include <thread>
#include <vector>

#include "eenv/eenv.hpp"

void set_environment(std::string_view name, std::string_view value) {
#ifdef _WIN32
    _putenv_s(name.data(), value.data());
#else
    setenv(name.data(), value.data(), 1);
#endif
}

// This test, when run with ThreadSanitizer (-fsanitize=thread),
// verifies that concurrent calls:
// - do not introduce internal data races;
// - do not cause memory corruption;
// - do not rely on unsafe shared mutable state.

int main() {

    const std::string TOKEN_CONTENT{"eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9."
                                    "eyJzdWIiOiIxMjM0NTY3ODkwIiwi"
                                    "bmFtZSI6IkVudlRlc3RVc2VyIiwic2NvcGUiOiJhcGk6cmVhZCJ9."
                                    "SflKxwRJSMeKKF2QT4fwpMeJf36POk6yJV_adQssw5c"};
    std::vector<std::thread> workers;

    struct[[= env::case_insensitive]] ConcurrentSettings {
        std::string token;
    };

    set_environment("TOKEN", TOKEN_CONTENT);

    constexpr int thread_count = 32;
    constexpr int iterations = 1000;

    for (size_t t = 0; t < thread_count; ++t) {
        workers.emplace_back([expected = TOKEN_CONTENT]() {
            for (size_t i = 0; i < iterations; ++i) {
                auto cfg = eenv::from_env<ConcurrentSettings>();

                //
            }
        });
    }

    for (auto &t : workers)
        t.join();

    std::cout << "Concurrent environment reads succeeded.\n";
}
