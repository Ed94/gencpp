# Documentation

All the library code is contained in two files: `gen.hpp` and `gen.cpp`

## gen.hpp

Feature Macros:

* `GENCPP_ROLL_OWN_DEPENDENCIES` : Optional override so that user may define the dependencies themselves.
* `GEN_DEFINE_LIBRARY_CORE_CONSTANTS` : Optional typename codes as they are non-standard to C/C++ and not necessary to library usage
* `GEN_FEATURE_PARSING` : Defines the parse constructors
* `GEN_FEATURE_EDITOR` : Defines the file editing features for changing definitions based on ASTs
* `GEN_FEATURE_SCANNER` : Defines the file scanning features for generating ASTs

Due to the design of `gen.hpp` to support being written alongside runtime intended code (in the same file), all the code is wrapped in a `gen_time` `#ifdef` and then wrapped further in a `gen` namespace to avoid pollution of the global scope.

*Note: Its possible with the scanner feature to support parsing runtime files that use "generic" macros or identifiers with certain patterns.  
This can be used to auto-queue generation of dependent definitions for the symbols used.*

### Organization

Dependencies : Mostly from the c-zpl library.

log_failure definition : based on whether to always use fatal on all errors

Major enum definitions and their associated functions used with the AST data

* `ECode` : Used to tag ASTs by their type
* `EOperator` : Used to tag operator overloads with thier op type
* `ESpecifier` : Used with specifier ASTs for all specifiers the user may tag an associated
AST with.
* `AccessSpec` : Used with class and struct ASTs to denote the public, protected, or private fields.
* `EnumT` : Used with def_enum to determine if constructing a regular enum or an enum class.
* `ModuleFlag` : Used with any valid definition that can have export or import related keywords assoicated with it.

#### Data Structures

`StringCache` : Hash table for cached strings. (`StringCached` typedef used to denote strings managed by it)

`AST` : The node data strucuture for the code.
`Code` : Wrapper for `AST` with functionality for handling it appropriately.

#### Gen Interface

First set of fowards are either backend functions used for various aspects of AST generation or configurating allocators used for different containers.

Interface fowards defined in order of: Upfront, Parsing, Untyped.

From there forwards for the File handlers are defined: Builder, Editor, Scanner.

#### Macros

General helper macros are defined along with the optional DSL macros.

#### Constants

Constants including optional ones are defined.

#### Inlines

Inlined functions related to the AST datatype that required forwards for gen interface functions are defined.

## gen.cpp

* Dependencies
* Static data
* AST Body case macros are next.
* AST implementation
* Gen interface begins with its `init`, `deinit`, etc.. Until `make_code_entires`
* operator__validate defined, which will be used to verify if an operator definition is constructible.
* Allocator interface for data arrays, code pool, code entries, string arenas, and string table.
* Helper macros used throughout the constructor API
* Upfront constructors, following the same order as shown in the header.
* Parsing constructors, it defines its own lexer, and has many helper functions for parsing not exposed through the header.
* Untyped constructors
* Builder
* Editor
* Scanner
