# Documentation

The library is fragmented into a series of headers and source files meant to be scanned in and then generated to a standard target format, or a user's desires.

Standard formats:

* **base**: Files are in granular pieces separated into four directories:
  * **dependencies**: Originally from the c-zpl library and modified thereafter.
  * **components**: The essential definitions of the library.
  * **helpers**: Contains helper functionality used by base and other libraries to regenerate or generate the other library formats.
    * `base_codegen.hpp`: Helps with self-hosted code generation of enums, and operator overload inlines of the code types.
    * `<push/pop>.<name>.inline.<hpp>`: macros that are meant to be injected at specific locations of the library.
    * `misc.hpp`:
    * `undef.macros.h`: Undefines all macros from library that original were intended to leak into user code.
  * **auxillary**: Non-essential tooling:
    * `Builder`: Similar conceptually to Jai programming language's *builder*, just opens a file and prepares a string buffer to serialize code into (`builder_print`, `builder_print_fmt`). Then write & close the file when completed (`builder_write`).
    * **`Scanner`**: Interface to load up `Code` from files two basic funcctions are currently provided.
      * `scan_file`: Used mainly by the library format generators to directly scan files into untyped `Code` (raw string content, pre-formatted no AST parsed).
      * `parse_file`: Used to read file and then parsed to populate a `CodeBody` AST.
* **gen_segemetned**: Dependencies go into gen.dep.{hpp/cpp} and components into gen.{hpp/cpp}
* **gen_singleheader**: Everything into a single file: gen.hpp
* **gen_unreal_engine**: Like gen_segemented but the library is modified slightly to compile as a thirdparty library within an Unreal Engine plugin or module.
* **gen_c_library**: The library is heavily modifed into C11 compliant code. A segemented and single-header set of variants are generatd.


Code not making up the core library is located in `auxiliary/<auxiliary_name>.<hpp/cpp>`. These are optional extensions or tools for the library.

Feature Macros:

* `GEN_DEFINE_ATTRIBUTE_TOKENS` : Allows user to define their own attribute macros for use in parsing.
  * This is auto-generated if using the bootstrap or single-header generation
  * *Note: The user will use the `AttributeTokens.csv` when the library is fully self-hosting.*
* `GEN_DEFINE_LIBRARY_CORE_CONSTANTS` : Optional typename codes as they are non-standard to C/C++ and not necessary to library usage
* `GEN_DONT_ENFORCE_GEN_TIME_GUARD` : By default, the library ( gen.hpp/ gen.cpp ) expects the macro `GEN_TIME` to be defined, this disables that.
* `GEN_ENFORCE_STRONG_CODE_TYPES` : Enforces casts to filtered code types.
* `GEN_EXPOSE_BACKEND` : Will expose symbols meant for internal use only.
* `GEN_ROLL_OWN_DEPENDENCIES` : Optional override so that user may define the dependencies themselves.
* `GEN_DONT_ALLOW_INVALID_CODE` (Not implemented yet) : Will fail when an invalid code is constructed, parsed, or serialized.
* `GEN_C_LIKE_PP` : Will prevent usage of function defnitions using references and structs with member functions.
Structs will still have user-defined operator conversions, for-range support, and other operator overloads

*Note: A variant of the C++ library could be generated where those additonal support features are removed (see gen_c_library implementation for an idea of how)*

## On multi-threading

Currently unsupported. I want the library to be *stable* and *correct*, with the addition of exhausting all basic single-threaded optimizations before I consider multi-threading.

## Extending the library

This library is relatively very small (for parsing C++), and can be extended without much hassle.

The convention you'll see used throughout the interface of the library is as follows:

1. Check name or parameters to make sure they are valid for the construction requested
2. Create a code object using `make_code`.
3. Populate immediate fields (Name, Type, ModuleFlags, etc)
4. Populate sub-entires using `add_entry`. If using the default serialization function `to_string`, follow the order at which entires are expected to appear (there is a strong ordering expected).

Names or Content fields are interned strings and thus showed be cached using `get_cached_string` if its desired to preserve that behavior.

`def_operator` is the most sophisticated upfront constructor as it has multiple permutations of definitions that could be created that are not trivial to determine if valid.

The parser is documented under `docs/Parsing.md` and `docs/Parser_Algo.md`. Extending it is more serious, but resolution of a parse for a given internal parse procedure is well documented.

## A note on compilation and runtime generation speed

The library is designed to be fast to compile and generate code at runtime as fast as resonable possible on a debug build.
Its recommended that your metaprogam be compiled using a single translation unit (unity build).
