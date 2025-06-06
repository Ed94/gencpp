## Navigation

# base

[Top](../Readme.md)

* [docs](../docs/Readme.md)

The library is fragmented into a series of headers and source files meant to be scanned in and then generated to a standard target format, or a user's desires.

If using the library's provided build scripts:

```ps1
.\build.ps1 <compiler> <debug or omit> base
```

## Content Overview

* **base**: Files are in granular pieces separated into four directories:
  * **dependencies**: Originally from the c-zpl library and modified thereafter.
  * **components**: The essential definitions of the library.
  * **helpers**: Contains helper functionality used by base and the variant library generators.
    * `base_codegen.hpp`: Helps with self-hosted code generation of enums, and operator overload inlines of the code types.
    * `<push/pop>.<name>.inline.<hpp>`: macros that are meant to be injected at specific locations of the library file/s.
    * `misc.hpp`: Misc functionality used by the library generation metaprograms.
    * `undef.macros.h`: Undefines all macros from library.
  * **auxiliary**: Non-essential tooling:
    * `Builder`: Similar conceptually to Jai programming language's *builder*, just opens a file and prepares a string buffer to serialize code into (`builder_print`, `builder_print_fmt`). Then write & close the file when completed (`builder_write`).
    * **`Scanner`**: Interface to load up `Code` from files two basic functions are currently provided.
      * `scan_file`: Used mainly by the library format generators to directly scan files into untyped `Code` (raw string content, pre-formatted no AST parsed).
      * `parse_file`: Used to read file and then parsed to populate a `CodeBody` AST.
      * CSV parsing via one or two columns simplified.
* **gen_segemetned**: Dependencies go into gen.dep.{hpp/cpp} and components into gen.{hpp/cpp}
* **gen_singleheader**: Everything into a single file: gen.hpp
* **gen_unreal_engine**: Like gen_segmented but the library is modified slightly to compile as a thirdparty library within an Unreal Engine plugin or module.
* **gen_c_library**: The library is heavily modifed into C11 compliant code. A segmented and single-header set of variants are generated.

Code not making up the core library is located in `auxiliary/<auxiliary_name>.<hpp/cpp>`. These are optional extensions or tools for the library.

## Dependencies

The project has no external dependencies beyond:

* `errno.h`
* `stat.h`
* `stdarg.h`
* `stddef.h`
* `stdio.h`
* `copyfile.h` (Mac)
* `types.h`    (Linux)
* `sys/man.h`  (Linux)
* `fcntl.h`    (POSXIX Filesystem)
* `unistd.h`   (Linux/Mac)
* `intrin.h`   (Windows)
* `io.h`       (Windows with gcc)
* `windows.h`  (Windows)

Dependencies for the project are wrapped within `GENCPP_ROLL_OWN_DEPENDENCIES` (Defining it will disable them).
The majority of the dependency's implementation was derived from the [c-zpl library](https://github.com/zpl-c/zpl).

See the following files for any updates:

* [`platform.hpp`](./dependencies/platform.hpp)
* [`src_start.cpp`](./dependencies/src_start.cpp)
* [`filesystem.cpp`](./dependencies/filesystem.cpp)
* [`memory.cpp`](./dependencies/memory.cpp)

## Conventions

This library was written in a subset of C++ where the following are not used at all:

* RAII (Constructors/Destructors), lifetimes are managed using named static or regular functions.
* Language provide dynamic dispatch, RTTI
* Object-Oriented Inheritance
* Exceptions

Polymorphic & Member-functions are used as an ergonomic choice, along with a conserative use of operator overloads.
The base library itself does not use anything but C-like features to allow for generating a derviative compatiable with C.

Member function support or free-functions with reference object passing are wrapped in `! GEN_C_LIKE CPP` preprocess conditionals.

## C++ template usage

There are only 4 template definitions in the entire library (C++ versions). (`Array<Type>`, `Hashtable<Type>`, `swap<Type>`, and `tmpl_cast<CodeT>(CodeT code)`)

Two generic templated containers are used throughout the library:

* `template< class Type> struct Array`
* `template< class Type> struct HashTable`

`tmpl_cast<CodeT>(CodeT code)` is just an alternative way to explicitly cast to code. Its usage is wrapped in a macro called `cast` for the base library (needed for interoperability with C).

`template< class Type> swap( Type& a, Type& b)` is used over a macro.

Otherwise the library is free of any templates.

## Macro usage

Since this is a meta-programming library, it was desired to keep both templates and macros (especially macros) usage very limited.

Most macros are defined within [macros.hpp](./dependencies/macros.hpp). 

The most advanced macro usage is `num_args` which is a helper for counting the number of arguments of another macro.

Any large macros used implementing the gen interface or parser are going to be phased out in favor of just forcinlined functions.  
*(Unless there is a hot-path that requires them)*

The vast majority of macros should be single-line subsitutions that either add:

* Improvements to searching
* Intentionality of keyword usage
* A  feature that only the preprocessor has (ex: function name reflection or stringifying)
* Compatibility of statements or expressions bewteen C & C++ that cannot be parsed by gencpp itself.
* Masking highly verbose syntax (the latter is getting phased out).

[gen_c_library](../gen_c_library/) has the most advanced set of macros for c11's generic selection.

* A significant amount of explicit code geneeration is utilized to keep abuse of the preprocessor to the absolute minimum.
* There is a heavy set of documentation inlined wth them; their naming is also highly verbose and explicit.
* See its documentation for more information.

## On base code generation

There are ***five*** header files which are automatically generated using [base_codegen.hpp](./helpers/base_codegen.hpp) by [base.cpp](./base.cpp). They are all located in [components/gen](./components/gen/).

* [ecodetypes.hpp](./components/gen/ecode.hpp): `CodeType` enum definition and related implementation. Generation is based off of [ECodeType.csv](./enums/ECodeTypes.csv).
* [especifier.hpp](./components/gen/especifier.hpp): `Specifier` enum definition, etc. Generated using [ESpecifier.csv](./enums/ESpecifier.csv).
* [eoperator.hpp](./components/gen/eoperator.hpp): `Operator` enum definition, etc. Generated using [EOperator.hpp](./enums/EOperator.csv).
* [etoktype.cpp](./components/gen/etoktype.cpp): `TokType` enum defininition, etc. Used by the lexer and parser backend. Uses two csvs:
  * [ETokType.csv](./enums/ETokType.csv): Provides the enum entries and their strinng ids.
  * [AttributeTokens.csv](./enums/AttributeTokens.csv): Provides tokens entries that should be considered as attributes  by the lexer and parser. Sspecfiically macro attributes such as those use for exporting symbols.
* [ast_inlines.hpp](./components/gen/ast_inlines.hpp): Member trivial `operator` definitions for C++ code types. Does not use a csv.

[misc.hpp](./helpers/misc.hpp): Has shared functions used by the library generation meta-programs throughout this codebase.

If using the library's provided build scripts:

```ps1
.\build.ps1 <compiler> <debug or omit> base
```

Will refresh those files.

## On multi-threading

Currently unsupported. I want the library to be *stable* and *correct*, with the addition of exhausting all basic single-threaded optimizations before I consider multi-threading.

## Extending the library

This library is relatively very small (for parsing C++), and can be extended without much hassle.

The convention you'll see used throughout the upfront interface of the library is as follows:

1. Check name or parameters to make sure they are valid for the construction requested
2. Create a code object using `make_code`.
3. Populate immediate fields (Name, Type, ModuleFlags, etc)
4. Populate sub-entires using `add_entry`. If using the default serialization function `to_strbuilder`, follow the order at which entires are expected to appear (there is a strong ordering expected).

Names or Content fields are interned strings and thus showed be cached using `cache_str` if its desired to preserve that behavior.

`def_operator` is the most sophisticated upfront constructor as it has multiple permutations of definitions that could be created that are not trivial to determine if valid.

The parser is documented under [`docs/Parsing.md`](../docs/Parsing.md) and [`docs/Parser_Algo.md`](../docs/Parser_Algo.md). Read that and the entire library if you want to extend it.

### Attributes

To add additional macro attributes, all that has to be done is modifying [`AttributeTokens.csv`](./enums/AttributeTokens.csv).

### Specifiers

To add additional macro specifiers, the following needs to be done:

1. Adjust [especifier.hpp](./components/gen/especifier.hpp)
2. Adjust [etoktype.cpp](./components/gen/etoktype.cpp)
3. Adjust [parser_case_macros.cpp](./components/parser_case_macros.cpp)

If the specifier is a new trailing specifier on function definitions:

Head into [base_codegen.hpp](./helpers/base_codegen.hpp): `gen_especifier`. There will be an `is_trailing` function that needs to be adjusted with an additional case for the user's new trailing specifier.

### Code Types

These require the following to be handled to the equivalent extent as the other types:

1. Adjust [ECodeTypes.csv](./enums/ECodeTypes.csv) with the new types
2. Define a new `AST_<Name>` and `Code<Name>`. See
    * [ast.hpp](./components/ast.hpp): Initial forwards and user defined conversion for Code.
    * [ast_types.hpp](./components/ast_types.hpp): Define the `AST_<Name>` struct.
    * [code_types.hpp](./components/code_types.hpp): Defne the `CodeType` struct. If its needs an iterator see: `struct CodeBody` & `struct CodeParams`.
3. [ast_case_macros.cpp](./components/ast_case_macros.cpp): Review cases here if the new code type needs to be considered.
4. [ast.cpp](./components/ast.cpp): Need to review
    * `code_debug_str`
    * `code_is_equal`
    * `code_to_strbuilder_ref`
    * `code_validate_body`
5. [code_serialization.cpp](./components/code_serialization.cpp): Define serialization here.
6. [inlines.hpp](./components/inlines.hpp): Any inline definitions for the `struct Code<Name>` are defined here.
7. [interface.cpp](./components/interface.hpp): Define the `Code<Name>` upfront and parsing interface.
8. [interface.upfront.cpp](./components/interface.upfront.cpp): Define the upfront constructor implementation.
9. [interface.parsing.cpp](./components/interface.parsing.cpp): Define the parsing interface implementation.
10. [lexer.cpp](./components/lexer.cpp): Adjust the lexer as needed.
11. [parser.cpp](./components/parser.cpp): Adjust the parser as needed.

## A note on compilation and runtime generation speed

The library is designed to be fast to compile and generate code at runtime as fast as possible on a debug build.
Its recommended that your metaprogam be compiled using a single translation unit (unity build).

## Whats with the expression / executions support #ifd and enums?

The library is a *work in progress* and those are unfinished hypotheticals for adding the ability to manage or parse the AST of expresions or execution scope code.
They are entirely untested and not meant to be used yet, futher there is no parsing support or an upfront interface for what CodeTypes are defined so far.
