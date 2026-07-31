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
// from_env<T>() default-constructs T, walks its non-static data members via
// C++26 static reflection (P2996), and for each member whose name (upper-
// cased) matches an environment variable, converts and assigns it. Members
// with no matching variable are left untouched, so ordinary default member
// initializers ("int batch_size = 10000;") behave as defaults.
//
// Unknown/extra environment variables are inherently ignored: the walk goes
// field -> env var, never env var -> field, so there is no "unknown field"
// case to police here.
//
// OPTIONAL ANNOTATIONS INTEGRATION
// ---------------------------------------------------------------------
// This header has no hard dependency on eenv/annotations.hpp. Used alone,
// from_env<T>() uses its own defaults: field name upper-cased verbatim,
// case-sensitive std::getenv() lookup, nothing required, no per-field
// defaults beyond the struct's own member initializers.
//
// If a translation unit includes "eenv/annotations.hpp" BEFORE this header,
// the EENV_HAS_ANNOTATIONS macro will already be defined by the time this
// file is preprocessed, and from_env<T>() below switches to resolving each
// field's env-var name, case-sensitivity, required-ness and default value
// through the annotation helpers in eenv::detail instead. Include order
// matters here because this is a preprocessor-time switch, not a runtime
// one:
//
//   #include "eenv/annotations.hpp"   // must come first
//   #include "eenv/settings.hpp"      // then this
//

#include <cstdlib>
#include <meta>
#include <optional>
#include <string>

#include "eenv/constraints.hpp"
#include "eenv/convert.hpp"
#include "eenv/errors.hpp"
#include "eenv/optional.hpp"
#include "eenv/secret.hpp"

namespace eenv {

namespace detail {

// Converts a C++ member name to its default environment-variable name.
//
// Example:
//   "storage_account_key" -> "STORAGE_ACCOUNT_KEY"
//
// Used only when compile-time annotations are not enabled.
// When annotations are available, name resolution is delegated to
// eenv::detail::env_name_for().
inline std::string to_env_name(std::string_view member_name) {
    // ASCII-only uppercase, matching eenv::detail::env_name_for() in
    // annotations.hpp, so both code paths produce identical env-var names
    // regardless of locale (std::toupper is locale-dependent).
    std::string out;
    out.reserve(member_name.size());
    for (char c : member_name) {
        out.push_back((c >= 'a' && c <= 'z') ? static_cast<char>(c - 'a' + 'A') : c);
    }
    return out;
}

inline std::optional<std::string> read_env(const std::string &name) {
    if (const char *value = std::getenv(name.c_str())) {
        return std::string(value);
    }
    return std::nullopt;
}

} // namespace detail

template <typename T> T from_env() {
    T result{};
    std::vector<FieldError> errors;

    static constexpr auto members =
        std::define_static_array(std::meta::nonstatic_data_members_of(^^T, std::meta::access_context::current()));

    template for (constexpr auto member : members) {
        constexpr std::string_view field_name = std::meta::identifier_of(member);

    using MemberType = typename[:std::meta::type_of(member):];

#if defined(EENV_HAS_ANNOTATIONS)
        // A member-level env::required directly on a std::optional<T> field
        // contradicts the field's own type. As a result, it fails at compile time 
        // rather than silently pick a winner. A class-level env::required reaching
        // this same field is fine and is handled below, not here: it's
        // meant as a general default, and std::optional<T> exempting
        // itself from it is the intended use of that mechanism, not a bug.
        static_assert(!(is_optional_v<MemberType> && detail::annotation_of<env::required_t>(member).has_value()),
                      "env::required directly on a std::optional<T> member is contradictory: "
                      "std::optional already encodes 'this may be absent' at the type level. "
                      "Remove env::required from this member (a class-level env::required is "
                      "fine and is silently ignored for std::optional<T> members), or change "
                      "the member's type.");
#endif

#if defined(EENV_HAS_ANNOTATIONS)
        const std::string env_name = detail::env_name_for(member);
        const bool case_sensitive = detail::is_case_sensitive(member, ^^T);
        auto raw = get_env(env_name.c_str(), case_sensitive);
#else
        const std::string env_name = detail::to_env_name(field_name);
        auto raw = detail::read_env(env_name);
#endif

        if (!raw) {
#if defined(EENV_HAS_ANNOTATIONS)
            if (auto dv = detail::annotation_of<env::default_value>(member)) {
                raw = std::string(dv->value.view());
            } else if (detail::is_required(member, ^^T) && !is_optional_v<MemberType>) {
                errors.push_back(
                    FieldError{std::string(field_name), "required environment variable '" + env_name + "' is not set"});
                continue;
            } else {
                continue;
            }
#else
            continue;
#endif
        }

        try {
            if constexpr (is_secret_v<MemberType>) {
                using Inner = secret_inner_t<MemberType>;
                result.[:member:].set(Converter<Inner>::convert(field_name, *raw));
            } else if constexpr (is_range_v<MemberType>) {
                using Inner = range_inner_t<MemberType>;
                result.[:member:].set(Converter<Inner>::convert(field_name, *raw));
            } else if constexpr (is_optional_v<MemberType>) {
                using Inner = optional_inner_t<MemberType>;
                result.[:member:] = Converter<Inner>::convert(field_name, *raw);
            } else {
                result.[:member:] = Converter<MemberType>::convert(field_name, *raw);
            }
        } catch (const ConversionError &e) {
            errors.push_back(FieldError{std::string(field_name), e.what()});
        }
    }

    if (!errors.empty()) {
        throw SettingsError(std::move(errors));
    }

    return result;
}

} // namespace eenv
