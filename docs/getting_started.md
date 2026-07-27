# Getting Started

## Compiler support

`eenv` requires a C++26 compiler supporting static reflection (P2996).  
At the time of writing, GCC 16 (or later) is the recommended compiler.  

The library is header-only and has no external dependencies.  

## Integrating eenv

# Getting Started

## Compiler support

`eenv` requires a C++26 compiler supporting static reflection (P2996).  
At the time of writing, GCC 16 (or later) is the recommended compiler.  

The library is header-only and has no external dependencies.  

## Building eenv

After cloning the repository, configure and build the project using one of the provided CMake presets.

```bash
git clone https://github.com/<owner>/eenv.git
cd eenv

cmake --preset=release
cmake --build --preset=release
```

The default configuration builds the library and the example programs.  
Unit tests can be enabled through the corresponding CMake option.

## CMake options

The following CMake options control the project configuration.

| Option | Default | Description |
| :----- | :-----: | :---------- |
| `EENV_BUILD_EXAMPLES` | `ON` | Build the example programs. |
| `EENV_BUILD_TESTS` | `OFF` | Build the unit tests. |
| `EENV_FETCH_GTEST` | `OFF` | Download GoogleTest with `FetchContent` if it is not found locally. |
| `EENV_WARNINGS_AS_ERRORS` | `ON` | Treat compiler warnings as errors. |

## Using eenv in a CMake project

eenv is a header-only library and exposes the CMake target `eenv::eenv`.

Add the library to your project using your preferred dependency management strategy (for example `add_subdirectory()`, `FetchContent`, or a package manager), then link your target against `eenv::eenv`.

```cmake
add_executable(my_application
    main.cpp
)

target_link_libraries(my_application
    PRIVATE
        eenv::eenv
)
```

## Basic usage 

```cpp
#include <eenv/eenv.hpp>
// Define a configuration schema
// Name your struct Settings or whatever you like
struct Settings {
    std::string host;
    int port;
    bool debug;
};

// The configuration schema is described as a regular C++ type.

// Load the configuration
auto settings = eenv::from_env<Settings>();
```

<b>That's all !</b>  

The library :  

- discovers members using reflection.  
- reads matching environment variables.  
- converts values.  
- validates the resulting object.  

## Next steps

See :

[Compile-time annotations](annotations.md)  
[Utility types](utility_types.md)


