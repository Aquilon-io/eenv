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

// clang-format off
struct Settings {
    [[= env::alias{"HTTP_PROXY"}]]
    [[= env::case_sensitive]] 
    std::string proxy;

    [[= env::default_value{"8080"}]] 
    int port; // or int port{8080}

    [[= env::required]] 
    std::string host;
};

struct [[= env::case_sensitive]] StrictSettings {

    // Inherits case_sensitive from the class
    std::string token;

    // Local override: back to insensitive
    [[= env::case_insensitive]] 
    std::string legacy_flag;
};
// clang-format on

template <class T> void print_meta(std::string_view label) {
    static constexpr auto meta = eenv::collect_field_meta<T>();

    std::cout << "== " << label << " ==\n";
    for (auto const &f : meta) {
        std::cout << "  " << f.identifier << " -> env=" << f.env_name << " case_sensitive=" << std::boolalpha
                  << f.case_sensitive << " required=" << f.required;
        if (f.default_value) {
            std::cout << " default=" << f.default_value;
        }
        std::cout << '\n';
    }
}

int main() {
    print_meta<Settings>("Settings");
    // proxy -> env=HTTP_PROXY  case_sensitive=true
    // port  -> env=PORT        case_sensitive=false  default=8080
    // host  -> env=HOST        case_sensitive=false  required=true

    std::cout << '\n';

    try {
        Settings s = eenv::from_env<Settings>();
        std::cout << "proxy           = " << s.proxy << '\n';
        std::cout << "port            = " << s.port << '\n';
        std::cout << "host            = " << s.host << '\n';

    } catch (eenv::SettingsError &e) {
        std::cerr << e.what() << '\n';
    }
    std::cout << '\n';

    print_meta<StrictSettings>("StrictSettings");
    //
    // token       -> env=TOKEN  case_sensitive=true  (inherited from the class)
    // legacy_flag               case_sensitive=false  (member override)

    // from_env<T>() uses the same annotations automatically, since
    // annotations.hpp was included before settings.hpp above:
    //

    //
    // Direct lookup, once a name is resolved:
    // auto v = eenv::get_env("HTTP_PROXY", /*case_sensitive=*/true);

    std::cout << '\n';
    try {
        StrictSettings s2 = eenv::from_env<StrictSettings>();

        if (!s2.token.empty()) {
            std::cout << "token            = " << s2.token << '\n';
        } else {
            std::cout << "token is not set " << '\n';
        }

        if (!s2.legacy_flag.empty()) {
            std::cout << "legacy_flag      = " << s2.legacy_flag << '\n';
        } else {
            std::cout << "legacy_flag is not set " << '\n';
        }

    } catch (eenv::SettingsError &e) {
        std::cerr << e.what() << '\n';
    }

    std::cout << '\n';
}
