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
// Type-directed conversion from a raw environment-variable string to a
// scalar C++ value. Every converter is "fail loud": ambiguous or partial
// parses (e.g. atoi("bonjour") == 0) are rejected rather than silently
// coerced.
//
// Unsupported types are a compile error (primary template left
// undefined) rather than a silent no-op, so adding a new field type to a
// Settings struct without teaching env_settings how to parse it fails at
// build time, not at runtime.
//
#include <algorithm>
#include <charconv>
#include <concepts>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <system_error>

#include "eenv/errors.hpp"

namespace eenv {

// consteval-safe integer-to-string. std::to_string is not constexpr, even
// under GCC 16 / C++26 libstdc++, so anything that needs to format a
// number inside a consteval context (compile-time diagnostics, static
// assertions on default values, etc.) should reach for this instead of
// std::to_string. Runtime-only code (like Range::set below) can still use
// std::to_string directly -- this helper only exists for the consteval
// case.
constexpr std::string to_chars_string(long long value) {
    char buf[32];
    auto res = std::to_chars(buf, buf + sizeof(buf), value);
    return std::string(buf, res.ptr);
}

// Primary template: intentionally undefined. Specialize for each
// supported field type.
template <typename T> struct Converter;

template <> struct Converter<std::string> {
    static std::string convert([[maybe_unused]] std::string_view field, std::string_view raw) {
        return std::string(raw);
    }
};

template <typename T>
    requires std::integral<T> && (!std::same_as<T, bool>)
struct Converter<T> {
    static T convert([[maybe_unused]] std::string_view field, std::string_view raw) {
        T value{};
        const auto *begin = raw.data();
        const auto *end = raw.data() + raw.size();
        auto [ptr, ec] = std::from_chars(begin, end, value);
        if (ec != std::errc{} || ptr != end) {
            throw ConversionError("cannot convert '" + std::string(raw) + "' to an integral value");
        }
        return value;
    }
};
template <> struct Converter<double> {
    static double convert([[maybe_unused]] std::string_view field, std::string_view raw) {
        double value{};
        const auto *begin = raw.data();
        const auto *end = raw.data() + raw.size();
        auto [ptr, ec] = std::from_chars(begin, end, value);
        if (ec != std::errc{} || ptr != end) {
            throw ConversionError("cannot convert '" + std::string(raw) + "' to double");
        }
        return value;
    }
};

template <> struct Converter<bool> {
    static bool convert([[maybe_unused]] std::string_view field, std::string_view raw) {
        std::string lower(raw);
        std::ranges::transform(lower, lower.begin(), [](unsigned char c) { return std::tolower(c); });
        if (lower == "1" || lower == "true" || lower == "yes" || lower == "on") {
            return true;
        }
        if (lower == "0" || lower == "false" || lower == "no" || lower == "off") {
            return false;
        }
        throw ConversionError("cannot convert '" + std::string(raw) + "' to bool");
    }
};

template <> struct Converter<std::filesystem::path> {
    static std::filesystem::path convert([[maybe_unused]] std::string_view field, std::string_view raw) {
        return std::filesystem::path(std::string(raw));
    }
};

// std::optional<T>: an empty environment variable (VAR=) resets the field
// to nullopt instead of trying to parse an empty string as T. An
// *absent* variable is handled one level up, in from_env, by simply not
// touching the member -- so a defaulted optional stays at its default.
template <typename T> struct Converter<std::optional<T>> {
    static std::optional<T> convert(std::string_view field, std::string_view raw) {
        if (raw.empty()) {
            return std::nullopt;
        }
        return Converter<T>::convert(field, raw);
    }
};

} // namespace eenv
