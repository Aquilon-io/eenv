# Compile-time Annotations

Compile-time annotations customize how individual configuration fields are loaded, converted and validated.  
Annotations are expressed using standard C++26 attributes and are resolved entirely at compile time through static reflection (P2996).  
Annotations customize the behaviour of a field without changing its C++ type.

```cpp
struct Settings
{
    [[= env::required]]
    [[= env::alias{"HTTP_HOST"}]]
    std::string host;

    [[= env::default_value{"8080"}]]
    int port;
};
```

## Field-level and type-level annotations

Annotations can be applied either to an entire configuration type or to individual fields.  
Type-level annotations define the default behaviour for all fields.  

```cpp
[[= env::case_sensitive]]
struct StrictSettings
{
    // Inherits the type-level annotation.
    std::string token;

    // Local override.
    [[= env::case_insensitive]]
    std::string legacy_flag;
};
```

When both are present, field-level annotations take precedence over type-level annotations.  

## Available annotations

| Annotation | Description |
| :--------- | :---------- |
| `alias` | Maps a field to a different environment variable name. |
| `required` | Marks a field as mandatory. Loading fails if the corresponding environment variable is missing. |
| `default_value` | Provides a default value when the environment variable is not defined. |
| `case_sensitive` | Performs a case-sensitive lookup of the environment variable name. |
| `case_insensitive` | Performs a case-insensitive lookup of the environment variable name. |  

## Examples

### alias

```cpp
struct Settings
{
    [[= env::alias{"HTTP_PROXY"}]]
    std::string proxy;
};
```

The `proxy` field is loaded from the `HTTP_PROXY` environment variable.

---

### required

```cpp
struct Settings
{
    [[= env::required]]
    std::string host;
};
```

Loading fails if `HOST` is not defined.

---

### default_value

```cpp
struct Settings
{
    [[= env::default_value{"8080"}]]
    int port;
};
```

If `PORT` is not defined, `port` is initialized with `8080`.

---

### case_sensitive

```cpp
struct Settings
{
    [[= env::case_sensitive]]
    std::string host;
};
```

The environment variable name must match exactly.

---

### case_insensitive

```cpp
struct Settings
{
    [[= env::case_insensitive]]
    std::string host;
};
```

The environment variable lookup ignores character case.

---

