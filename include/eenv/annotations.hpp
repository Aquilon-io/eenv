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
// Optional companion to eenv/settings.hpp. Allows configuration types and
// their fields to carry compile-time annotations such as
// [[= env::alias{...}]], [[= env::required]] and
// [[= env::default_value{...}]], resolved through C++26 static reflection.
//
// INTEGRATION: this header is a soft, opt-in dependency of eenv/settings.hpp.
// If (and only if) you #include "eenv/annotations.hpp" BEFORE
// "eenv/settings.hpp" in a translation unit, from_env<T>() will pick up
// aliasing / case-sensitivity / required / default-value behaviour through
// the EENV_HAS_ANNOTATIONS macro defined below. Include settings.hpp on its
// own for the plain default behaviour (uppercased field name, case-sensitive
// lookup, nothing required, no defaults). See eenv/settings.hpp for details.
//

#define EENV_HAS_ANNOTATIONS 1

#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <meta>
#include <optional>

extern char **environ;

// ---------------------------------------------------------------------
// Annotation tags. Deliberately kept in their own short `env` namespace
// (sibling to `eenv`, not nested under it) so call sites read as
// [[= env::required]] rather than [[= eenv::env::required]].
// ---------------------------------------------------------------------
namespace env {

struct case_sensitive_t {};
inline constexpr case_sensitive_t case_sensitive{};

// Complement of case_sensitive: lets a member explicitly opt back into
// case-insensitive matching even when the enclosing class is annotated
// env::case_sensitive. Without this tag the member class override could
// only ever go one direction.
struct case_insensitive_t {};
inline constexpr case_insensitive_t case_insensitive{};

struct required_t {};
inline constexpr required_t required{};

// Fixed-capacity string used by annotation payloads.
//
// Current reflection implementations cannot reliably extract annotation
// payloads containing raw pointers (e.g. const char*). Storing characters
// by value keeps the payload structural and extractable at compile time.
inline constexpr std::size_t max_annotation_string_length = 63;

struct fixed_string {
    char data[max_annotation_string_length + 1]{};

    constexpr fixed_string() = default;

    template <std::size_t N> constexpr fixed_string(const char (&str)[N]) {
        static_assert(N <= sizeof(data), "annotation string too long -- raise max_annotation_string_length");
        for (std::size_t i = 0; i < N; ++i) {
            data[i] = str[i];
        }
    }

    constexpr std::string_view view() const {
        std::size_t len = 0;
        while (len < sizeof(data) && data[len] != '\0') {
            ++len;
        }
        return std::string_view(data, len);
    }
};

struct alias {
    fixed_string value;
};

struct default_value {
    fixed_string value;
};

} // namespace env

// ---------------------------------------------------------------------
// Annotation resolution via reflection.
// ---------------------------------------------------------------------
namespace eenv::detail {

// Returns at most one annotation of type T on a reflected entity (data
// member or struct/class). std::nullopt if absent.
template <class T> consteval std::optional<T> annotation_of(std::meta::info entity) {
    for (std::meta::info a : std::meta::annotations_of(entity)) {
        // Compare unqualified types to ignore cv-qualification on annotation payloads.
        if (std::meta::remove_cv(std::meta::type_of(a)) == std::meta::remove_cv(^^T)) {
            return std::meta::extract<T>(a);
        }
    }
    return std::nullopt;
}

// Resolves the effective case-sensitivity of a member.
// Precedence: member annotation > class annotation > default (insensitive).
consteval bool is_case_sensitive(std::meta::info member, std::meta::info klass) {
    if (annotation_of<env::case_insensitive_t>(member)) {
        return false;
    }
    if (annotation_of<env::case_sensitive_t>(member)) {
        return true;
    }

    if (annotation_of<env::case_insensitive_t>(klass)) {
        return false;
    }
    if (annotation_of<env::case_sensitive_t>(klass)) {
        return true;
    }

    return false;
}

// Returns the environment-variable name associated with a member.
//
// Uses an explicit alias when present; otherwise derives the name by
// upper-casing the member identifier at compile time.
consteval const char *env_name_for(std::meta::info member) {
    if (auto a = annotation_of<env::alias>(member)) {
        return std::define_static_string(std::string(a->value.view()));
    }
    std::string name(std::meta::identifier_of(member));
    // Identifiers are ASCII, so a manual uppercase loop is constexpr-friendly
    // and locale-independent.
    for (char &c : name) {
        if (c >= 'a' && c <= 'z') {
            c = static_cast<char>(c - 'a' + 'A');
        }
    }
    return std::define_static_string(name);
}

} // namespace eenv::detail

// ---------------------------------------------------------------------
// Runtime environment-variable lookup.
// ---------------------------------------------------------------------
namespace eenv {

// case_sensitive == true  : plain getenv(), exact match.
// case_sensitive == false : scans `environ`, comparing the name case-insensitively.
inline std::optional<std::string> get_env(const char *name, bool case_sensitive) {
    if (case_sensitive) {
        if (const char *v = std::getenv(name)) {
            return std::string(v);
        }
        return std::nullopt;
    }

    const std::string_view target(name);
    for (char **e = environ; *e != nullptr; ++e) {
        std::string_view entry(*e);
        auto eq = entry.find('=');
        if (eq == std::string_view::npos)
            continue;

        std::string_view key = entry.substr(0, eq);
        if (key.size() != target.size())
            continue;

        bool match = std::equal(key.begin(), key.end(), target.begin(), target.end(),
                                [](unsigned char a, unsigned char b) { return std::tolower(a) == std::tolower(b); });
        if (match) {
            return std::string(entry.substr(eq + 1));
        }
    }
    return std::nullopt;
}

// ---------------------------------------------------------------------
// Per-field metadata, computed at compile time. Mainly useful for
// introspection/debugging (see examples/annotation_example.cpp); from_env<T>()
// itself calls the eenv::detail helpers above directly, field by field,
// rather than going through this array (see eenv/settings.hpp).
// ---------------------------------------------------------------------
struct field_meta {
    std::string_view identifier; // C++ member name
    const char *env_name;        // resolved environment variable name
    bool case_sensitive;
    bool required;
    const char *default_value; // nullptr if none
};

template <class T> consteval auto collect_field_meta() {
    constexpr auto members =
        std::define_static_array(std::meta::nonstatic_data_members_of(^^T, std::meta::access_context::current()));
    constexpr std::size_t n = members.size();

    std::array<field_meta, n> result{};
    for (std::size_t i = 0; i < n; ++i) {
        std::meta::info m = members[i];
        auto dv = detail::annotation_of<env::default_value>(m);
        result[i] = field_meta{
            .identifier = std::meta::identifier_of(m),
            .env_name = detail::env_name_for(m),
            .case_sensitive = detail::is_case_sensitive(m, ^^T),
            .required = detail::annotation_of<env::required_t>(m).has_value(),
            .default_value = dv ? std::define_static_string(std::string(dv->value.view())) : nullptr,
        };
    }
    return result;
}

} // namespace eenv
