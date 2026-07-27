# Utility Types

Utility types extend the semantics of configuration values by introducing dedicated C++ types.  
Unlike compile-time annotations, utility types are part of the declared C++ type.  

## Available utility types

| Type | Description |
| :--- | :---------- |
| `Secret<T>` | Marks a value as sensitive. The underlying value participates normally in loading and validation but is hidden by formatting and diagnostic utilities. |
| `Range<T, Min, Max>` | Constrains a value to a compile-time range. Loading fails if the converted value falls outside the specified bounds. |

## Secret<T>

`Secret<T>` is intended for sensitive configuration values such as passwords, API keys or access tokens.  

```cpp
struct Settings
{
    eenv::Secret<std::string> storage_account_key;
};
```

The wrapped value behaves like its underlying type during loading and validation, while formatting and diagnostic utilities automatically redact its contents.  

---

## Range<T, Min, Max>

`Range<T, Min, Max>` constrains a value to a compile-time range.  

```cpp
struct Settings
{
    eenv::Range<int, 1, 50000> batch_size{10000};
};
```

The value is converted to `T` and validated against the specified bounds. Loading fails if the value lies outside the allowed range.  
The default constructor value (`10000` in this example) is used when no value is provided and no other default is specified.  