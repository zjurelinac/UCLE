# UCLE environment coding convention (primarily for C++)

## Basic code style
A combination of K&R and Stroustrup with minor variations:

- **Functions** have opening braces on a separate line
- **One-liner** functions are allowed, but not required
- **Loop** and **conditional** braces are on the same line
- **For single-statement if's and loops**, braces aren't required
- **Class visibility levels** (public/private/protected) are indented
- **Use empty lines** to separate related statements in a meaningful group
  (for function and method bodies)
- **Use spaces** for indentation, having *4 spaces* for a single indent level

## Coding principles
Preferred ways of writing code:

- **Use of `auto`** instead of explicit types is encouraged (when possible)
- **Use `using`** to make complex type names more readable
- **Use `const` specifier** for everything that should be constant
- **Use `constexpr`** everywhere where it's sensible
- **Return from functions early**, and prefer testing for errors rather than for
  success
- **Do not use global variables**, if at all possible
- **Use exceptions** or **optional/variant** for error handling, never `errno`-style
  globals and macros

## Naming principles
Suggested rules for naming type aliases, classes, template parameters, functions,
methods, variables and constants:

- **Simple types** (ie. aliases for builtin types such as `int`, `char`, `bool`)
  should end with `_t` (e.g. `index_t` = `uint32_t`, `sbyte_t` = `sint8_t`)
- **Pointer types** should end with `_ptr` (eg. `device_ptr`) to match standard
  library convention (`shared_ptr`, `unique_ptr`)
- **Enums, POD structs and custom classes** should have an intuitive
  *snake_case* name expressing their meaning and having no prefix or suffix
- **Template parameter types** should be named either with uppercase letters
  (e.g. `T`, `U`), or using *CamelCase*, but in templated classes renamed into
  *snake_case* with a suffix `_type`
- **All variables, functions, methods and constants** should be named using
 `snake_case`
- **Private and protected members' names**, both of methods and variables,
  should end with `_` to make them easily distinguishable from public ones,
  but to retain as much readability as possible

