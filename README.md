# Extended Environment settings (eenv)

## A header only reflection-driven configuration library for modern C++.

Define your application's configuration as a C++ type. 

Most environment libraries help you retrieve individual variables.  
This library takes a different approach.  

Instead of querying environment variables throughout your code, you declare a typed configuration schema in C++. 
The library maps environment variables to your types, performs conversions, applies defaults and aliases, validates constraints, and returns a ready-to-use configuration object.

A configuration schema is simply described as a C++ type.

```c++
struct Settings
{
    int port;
    std::string host;
    bool debug;
    std::filesystem::path data_path;
};
```

Then

```c++
auto settings = eenv::from_env<Settings>();
```

The result is a valid instance of Settings.

## Key concepts

### Type-safe

Environment variables are converted directly into typed C++ values.  
The user never handles strings, instead, he receive directly a ```Settings``` object.  

### Declarative

The entire configuration schema is declared in one place as a regular C++ type.  
There is no separate configuration description, mapping table or registration code to maintain.

### Reflection-driven

No `register()` or `bind()` calls are required. Reflection automatically discovers the members of the configuration type.  

### Thread safety

`eenv` is thread-safe and reentrant. Concurrent calls to `eenv::load<T>()` and `from_env<T>()` are safe as long as the process environment is not modified concurrently by another thread (e.g. through `setenv`, `putenv`, or `unsetenv`).

The library does not perform internal synchronization around environment access.

## Schema extensions

### Compile-time annotations

Fields can be annotated to customize how values are loaded and validated.

- alias
- required
- default value
- case sensitivity

[Annotations example](examples/annotations_example.cpp) for details and [Compile-time annotations](docs/annotations.md) for a list of the available compile-time annotations.

### Utility types

The library also provides reusable types that extend the semantics of configuration values.

Examples include:

- `Secret<T>`
- `Range<T, Min, Max>`

[Basic example](examples/basic_example.cpp) and [Utility types](docs/utility_types.md) for details.

## Validation

Validation is performed in two stages:  

- type conversion and built-in constraints;
- user-defined validation.

The library never returns a partially valid configuration object.


## Examples

[basic_example.cpp](examples/basic_example.cpp) provides an initial overview of how to declare a configuration.  
[annotations_example.cpp](examples/annotations_example.cpp) provides additional examples of using compile-time annotations to extend a configuration.  

## Installation

Please consult [Getting Started](docs/getting_started.md) for further details on building and integrating the library as a third-party.

## Requirements

This library require a compiler that support C++26 reflection.  
Compatible compilers include GCC 16 or later.

This library is header-only.  
It requires no external dependencies.

