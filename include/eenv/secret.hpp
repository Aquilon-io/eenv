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

#pragma once

//
// Secret<T> marks a field as sensitive (API keys, connection strings,
// tokens, passwords). The loader still populates it normally from the
// environment, but the only way to get the real value out is the
// explicit reveal() call -- operator<< and any implicit-string-ish path
// print a fixed redaction placeholder instead. This makes "I accidentally
// logged a secret" a call-site decision (reveal()) rather than something
// that falls out of a default stream operator.
//
#include <ostream>
#include <type_traits>
#include <utility>

namespace eenv {

template <typename T> class Secret {
  public:
    Secret() = default;
    explicit Secret(T value) : value_(std::move(value)) {}

    // Deliberately explicit and loudly named: grepping a codebase for
    // ".reveal()" should be a viable way to audit every place a secret's
    // real value is touched.
    const T &reveal() const noexcept { return value_; }

    void set(T value) { value_ = std::move(value); }

    friend std::ostream &operator<<(std::ostream &os, const Secret &) { return os << "[REDACTED]"; }

  private:
    T value_{};
};

// --- trait helpers used by the reflection-based loader -----------------

template <typename U> struct is_secret : std::false_type {};

template <typename U> struct is_secret<Secret<U>> : std::true_type {};

template <typename U> inline constexpr bool is_secret_v = is_secret<U>::value;

template <typename U> struct secret_inner {
    using type = U;
};

template <typename U> struct secret_inner<Secret<U>> {
    using type = U;
};

template <typename U> using secret_inner_t = typename secret_inner<U>::type;

} // namespace eenv
