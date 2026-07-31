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

#include <optional>
#include <type_traits>

namespace eenv {

// Detects std::optional<T> member types, mirroring is_secret_v / is_range_v
// (see eenv/secret.hpp, eenv/constraints.hpp) so that from_env<T>() can
// unwrap the inner type before handing the raw string to Converter<Inner>.
//
// Absence stays free: T result{} default-constructs std::optional<T> to
// nullopt with zero extra code, so a member left untouched because its
// env var wasn't set is already correct.
template <typename T> struct is_optional : std::false_type {};
template <typename T> struct is_optional<std::optional<T>> : std::true_type {};
template <typename T> inline constexpr bool is_optional_v = is_optional<T>::value;

template <typename T> struct optional_inner;
template <typename T> struct optional_inner<std::optional<T>> { using type = T; };
template <typename T> using optional_inner_t = typename optional_inner<T>::type;

} // namespace eenv