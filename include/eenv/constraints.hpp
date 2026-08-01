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
// Supports integral scalars and floating-point scalars (float/double).
//
// Floating-point NTTPs (Min, Max as T) are legal since C++20, independent
// of P2996 reflection, so this compiles under -std=c++20 as well as the
// -freflection C++26 mode used elsewhere in eenv.
//

#include <charconv>
#include <concepts>
#include <string>

#include "eenv/errors.hpp"

namespace eenv {

// RangeScalar: integral or floating point, but not bool. std::integral<bool>
// is true, and a "range" over {false, true} is never a meaningful use of
// this constraint, so it's excluded explicitly.
template <typename T>
concept RangeScalar = (std::integral<T> || std::floating_point<T>) && !std::same_as<T, bool>;

template <RangeScalar T, T Min, T Max> class Range {
    // For integral T this is the only check needed.
    // For floating-point T, this also fires if Min or Max is NaN,
    // since any comparison against NaN is false
    // but the message below is clearer about *why* in that case.
    static_assert(Min <= Max, "Range: Min must be <= Max");

    // x == x is false only for NaN; deliberately not std::isnan
    // (not guaranteed constexpr-safe across libstdc++/GCC versions
    // same class of issue as consteval + C stdlib functions elsewhere in eenv).
    static_assert(!std::floating_point<T> || (Min == Min), "Range: Min must not be NaN");
    static_assert(!std::floating_point<T> || (Max == Max), "Range: Max must not be NaN");

  public:
    Range() : value_(Min) {}
    explicit Range(T value) { set(value); }

    T value() const noexcept { return value_; }
    operator T() const noexcept { return value_; }

    void set(T value) {
        // Written as "accept if inside, then negate"
        // NOT as(value < Min || value > Max). That De Morgan-equivalent form
        // silently accepts NaN for floating-point T, because NaN < x and
        // NaN > x are both false. This form rejects NaN correctly, since
        // (value >= Min) is false whenever value is NaN.
        if (!(value >= Min && value <= Max)) {
            throw ConversionError("value " + to_diagnostic_string(value) + " out of range [" +
                                  to_diagnostic_string(Min) + ", " + to_diagnostic_string(Max) + "]");
        }
        value_ = value;
    }

  private:
    // std::to_string(double) truncates to 6 decimals, which can hide the
    // actual reason a value was rejected (e.g. Max + 1ulp prints
    // identically to Max). std::to_chars gives the shortest round-trip
    // representation instead, which is what you want in a diagnostic.
    static std::string to_diagnostic_string(T v) {
        if constexpr (std::floating_point<T>) {
            char buf[64];
            auto res = std::to_chars(buf, buf + sizeof(buf), v);
            return std::string(buf, res.ptr);
        } else {
            return std::to_string(v);
        }
    }

    T value_;
};

// Trait helpers used by the reflection-based loader

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
