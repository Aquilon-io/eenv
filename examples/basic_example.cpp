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

#include <iostream>

#include "eenv/eenv.hpp"

//
// A basic example of using eev library.
//
// Reproduces the Settings example from this spec:
//
//   struct Settings {
//       std::string hostname;
//       std::string storage_account_key;
//       int batch_size = 10000;
//   };
//
// with the secret and bounds pieces wired in.
//
// Try it:
//   HOSTNAME=localhost
//   STORAGE_ACCOUNT_KEY=abc123
//   BATCH_SIZE=25000
//   BATCH_SIZE=bonjour                       # conversion error
//   BATCH_SIZE=9999999999999999999999999     # conversion error
//   BATCH_SIZE=999999999                     # range error
//

struct Settings {
    std::string hostname;
    eenv::Secret<std::string> storage_account_key;
    eenv::Range<int, 1, 50000> batch_size{10000};
};

int main() {
    try {
        Settings settings = eenv::from_env<Settings>();

        std::cout << "hostname              = " << settings.hostname << '\n';
        std::cout << "storage_account_key   = " << settings.storage_account_key << '\n';
        std::cout << "batch_size            = " << settings.batch_size.value() << '\n';
    } catch (const eenv::SettingsError &e) {
        std::cerr << e.what() << '\n';
        return 1;
    }

    return 0;
}
