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

#include <filesystem>

#include "eenv/constraints.hpp"

static_assert(eenv::RangeScalar<char>);
static_assert(eenv::RangeScalar<short>);
static_assert(eenv::RangeScalar<int>);
static_assert(eenv::RangeScalar<long>);
static_assert(eenv::RangeScalar<long long>);

static_assert(eenv::RangeScalar<float>);
static_assert(eenv::RangeScalar<double>);
static_assert(eenv::RangeScalar<long double>);

// Compile-time: bool must be rejected
static_assert(!eenv::RangeScalar<bool>);

static_assert(!eenv::RangeScalar<std::string>);
static_assert(!eenv::RangeScalar<std::filesystem::path>);
static_assert(!eenv::RangeScalar<void *>);
