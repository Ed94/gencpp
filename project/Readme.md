# Documentation

The core library is contained within `gen.hpp` and `gen.cpp`.
Things related to the editor and scanner are in their own respective files. (Ex: `gen.scanner.<hpp/cpp>` )

Dependencies are within `gen.dep.<hpp/cpp>`

The library is fragmented into a series of headers and sources files meant to be scanned in and then generated to a tailored format for the target
`gen` files.

Both libraries use *pre-generated* (self-hosting I guess) version of the library to then generate the latest version of itself.
(sort of a verification that the generated version is equivalent)

The default `gen.bootstrap.cpp` located in the project folder is meant to be produce a standard segmeneted library, where the components of the library
have relatively dedicated header and source files. With dependencies included at the top of the file and each header starting with a pragma once.

`gen.singleheader.cpp` in the single header folder with its own `meson.build` generates the library as a single header `gen.hpp`. 
Following the same convention seen in the gb, stb, and zpl libraries.

Use those to get a general idea of how to make your own tailored version.

If the naming convention is undesired, the `gencpp.refactor` script can be used with the [refactor]()

## gen.hpp

Feature Macros:

* `GEN_DONT_USE_NAMESPACE` : By default, the library is wrapped in a `gen` namespace, this will disable that expose it to the global scope.
* `GEN_DONT_ENFORCE_GEN_TIME_GUARD` : By default, the library ( gen.hpp/ gen.cpp ) expects the macro `GEN_TIME` to be defined, this disables that.
* `GEN_ROLL_OWN_DEPENDENCIES` : Optional override so that user may define the dependencies themselves.
* `GEN_DEFINE_LIBRARY_CORE_CONSTANTS` : Optional typename codes as they are non-standard to C/C++ and not necessary to library usage
* `GEN_ENFORCE_STRONG_CODE_TYPES` : Enforces casts to filtered code types.
* `GEN_EXPOSE_BACKEND` : Will expose symbols meant for internal use only.
* `GEN_Define_Attribute_Tokens` : Allows user to define their own attribute macros for use in parsing.

`GEN_USE_RECURSIVE_AST_DUPLICATION` is available but its not well tested and should not need to be used.  
If constructing ASTs properly. There should be no modification of ASTs, and thus this would never become an issue.  
(I will probably remove down the line...)

Due to the design of `gen.hpp` to support being written alongside runtime intended code (in the same file), all the code is wrapped in a `GEN_TIME` `#ifdef` and then wrapped further in a `gen` namespace to avoid pollution of the global scope.

*Note: Its possible with the scanner feature to support parsing runtime files that use "generic" macros or identifiers with certain patterns.  
This can be used to auto-queue generation of dependent definitions for the symbols used.*

### Organization

Dependencies : Mostly from the c-zpl library.

log_failure definition : based on whether to always use fatal on all errors

Major enum definitions and their associated functions used with the AST data

* `ECode` : Used to tag ASTs by their type
* `EOperator` : Used to tag operator overloads with their op type
* `ESpecifier` : Used with specifier ASTs for all specifiers the user may tag an associated
AST with.
* `AccessSpec` : Used with class and struct ASTs to denote the public, protected, or private fields.
* `EnumT` : Used with def_enum to determine if constructing a regular enum or an enum class.
* `ModuleFlag` : Used with any valid definition that can have export or import related keywords associated with it.

#### Data Structures

`StringCache` : Hash table for cached strings. (`StringCached` typedef used to denote strings managed by it)

`Code` : Wrapper for `AST` with functionality for handling it appropriately.  
`AST` : The node data structure for the code.  
`Code Types` : Codes with typed ASTs. Body, Param, and Specifier have unique implementation, the rest use `Define_CodeType`  
`AST Types` : Filtered AST definitions.  

#### Gen Interface

First set of forwards are either backend functions used for various aspects of AST generation or configurations allocators used for different containers.

Interface forwards defined in order of: Upfront, Parsing, Untyped.

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
