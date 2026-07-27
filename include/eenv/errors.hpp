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
// Error types for env_settings. A single failed field never aborts the
// whole load: conversion/validation errors are collected per field and
// reported together in one SettingsError.
//
#include <stdexcept>
#include <string>
#include <vector>

namespace eenv {

// Thrown by individual type converters / constraint wrappers when a raw
// value cannot be turned into (or does not satisfy) the target type.
// Intentionally carries no field name: the caller (from_env) attaches
// that context when it catches this, so converters stay reusable and
// their messages stay focused on "what was wrong with the value".
struct ConversionError : std::runtime_error {
    using std::runtime_error::runtime_error;
};

struct FieldError {
    std::string field;
    std::string message;
};

// Aggregates every FieldError produced while loading a settings struct.
class SettingsError : public std::runtime_error {
  public:
    explicit SettingsError(std::vector<FieldError> errors)
        : std::runtime_error(build_message(errors)), errors_(std::move(errors)) {}

    const std::vector<FieldError> &errors() const noexcept { return errors_; }

  private:
    static std::string build_message(const std::vector<FieldError> &errors) {
        std::string msg = "settings validation failed (" + std::to_string(errors.size()) + " error(s)):";
        for (const auto &e : errors) {
            msg += "\n  - " + e.field + ": " + e.message;
        }
        return msg;
    }

    std::vector<FieldError> errors_;
};

} // namespace eenv
