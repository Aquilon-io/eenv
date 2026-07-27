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
// Range<T, Min, Max> : a closed-interval bound baked into the field's type
// via non-type template parameters, checked once at assignment time.
// It only covers integral scalars for now.
//

#include <concepts>
#include <string>

#include "eenv/errors.hpp"

namespace eenv {

template <std::integral T, T Min, T Max> class Range {
    static_assert(Min <= Max, "Range: Min must be <= Max");

  public:
    Range() : value_(Min) {}
    explicit Range(T value) { set(value); }

    T value() const noexcept { return value_; }
    operator T() const noexcept { return value_; }

    void set(T value) {
        if (value < Min || value > Max) {
            // Runtime-only path (called from from_env at load time),
            // so plain std::to_string is fine here.
            // see convert.hpp for the consteval-safe equivalent.
            throw ConversionError("value " + std::to_string(value) + " out of range [" + std::to_string(Min) + ", " +
                                  std::to_string(Max) + "]");
        }
        value_ = value;
    }

  private:
    T value_;
};

// --- trait helpers used by the reflection-based loader -----------------

template <typename U> struct is_range : std::false_type {};

template <typename U, U Min, U Max> struct is_range<Range<U, Min, Max>> : std::true_type {};

template <typename U> inline constexpr bool is_range_v = is_range<U>::value;

template <typename U> struct range_inner {
    using type = U;
};

template <typename U, U Min, U Max> struct range_inner<Range<U, Min, Max>> {
    using type = U;
};

template <typename U> using range_inner_t = typename range_inner<U>::type;

} // namespace eenv
