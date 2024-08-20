<p align="center">
  <img src="img/logo.svg" alt="Tau Logo" width="80%">
</p>

<h2 align="center">Coding Guidelines for the Tau Project</h2>

Welcome to our coding guidelines. These guidelines are intended to ensure
consistent, readable, and maintainable code across the project. Adhering to
these guidelines will enhance collaboration and overall code quality.

## Definitions

1. **MUST**: Indicates an absolute requirement that must be followed without
   exceptions. Failure to comply may result in non-conformance.
2. **MUST NOT**: Denotes a prohibition or constraint that must be strictly
   adhered to. Any action falling under this category is forbidden.
3. **SHOULD**: Suggests a strong recommendation or best practice that is
   generally expected to be followed. Deviating from this recommendation is
   acceptable but should be justified.
4. **SHOULD NOT**: Advises against a particular action or practice. While not
   mandatory, there are valid reasons for avoiding such actions.
5. **RECOMMENDED**: Implies a favorable suggestion or guidance that is
   recommended to achieve optimal results. Compliance is not obligatory, but it
   is advisable.
6. **MAY**: Signifies an optional provision or choice. The described action or
   condition is allowed, but there are alternatives.

## Naming Conventions

1. All names **MUST** utilize the *snake_case* style, wherein words consist only
   of lowercase ASCII characters and are separated by underscores. (e.g., `foo_bar`)
2. The length of a name **SHOULD NOT** exceed 40 characters in length. If the
   truncation of the name would result in a loss of clarity, the developer
   **SHOULD** provide comments explaining the purpose of the named entity at its
   declaration.
3. Abbreviations **MAY** be used if they aid to make the code more concise but
   do not significantly reduce clarity or readability. (e.g., Abstract Syntax
   Tree - AST)
4. If entities conceptually form a hierarchy, parts of their names **SHOULD** be
   ordered accordingly. (e.g., `animal_mammal_carnivore_cat`)

### Variables

1. Variable names **MUST NOT** begin with an underscore. This ensures clarity
   and compatibility with programming language standards.
2. Global variables **MUST** have the `g_` prefix. This ensures an easy way of
   distinguishing between global and local variables.

### Types

1. User-defined data types **MUST** end with the `_t` suffix.
2. Function pointer types **MUST** end with the `_func_t` suffix.

### Enumerations

1. Enumeration constant names **MUST** utilize the *SCREAMING_SNAKE_CASE* style,
   wherein words consist only of uppercase ASCII characters and are separated by
   underscores. (e.g., `FOO_BAR`)

### Functions

1. Functions **MUST** begin with a prefix that is unique to the specific header
   or module which the function is declared in.
2. Getter and setter functions **MUST** begin with the module specific unique
   prefix, followed by `get` or `set`, and ending with the name of the accessed
   data. (e.g., `foo_get_bar` where `foo` is the unique prefix and `bar` is the
   accessed data)

### Preprocessor Macros

1. Macros **MUST** utilize the *SCREAMING_SNAKE_CASE* style.
2. Include guard macros **MUST** have the following format: `TAU_{MODULE NAME}_H`

## Formatting

### Source Files

1. Code **MUST** be indented using 2 spaces.
2. The length of a line **SHOULD NOT** exceed 80 characters.
3. Opening braces **MUST** be placed on the next line after the associated
   declaration or control statement.
4. Source files **MUST** be structured in the following order:
   1. Include guard
   2. Standard library `#include` directives
   3. Third-party library `#include` directives
   4. Local library `#include` directives
   5. `#define` directives
   6. `typedef` declarations
   7. Global variable declarations
   8. Function declarations

### Comments

1. Multi-line documentation comments **MUST** utilize the *Javadoc* style, which
   consists of a C-style comments block starting with two asterisks.
2. Single-line documentation comments **MUST** utilize the C++-style starting
   with an additional slash.
3. Doxygen commands **MUST** be used for documentation comments.
4. Doxygen commands **MUST** be prefixed by a backslash.
5. Doxygen commands **MUST** always be explicit in multi-line documentation
   comments.
6. Documentation comments **MUST** be provided for declarations in public header
   files.
7. Documentation comments **SHOULD** be provided for declarations in private
   header files and all source files.

### Functions

1. Functions **SHOULD** not exceed 20 lines in length, excluding the signature
   and braces.
2. Setter functions **MUST** take the accessed object as their first parameter.
