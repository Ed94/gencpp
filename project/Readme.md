# Documentation

The library is fragmented into a series of headers and sources files meant to be scanned in and then generated to a tailored format for the target
`gen` files.

The principal (user) files are `gen.hpp` and `gen.cpp`.  
They contain includes for its various components: `components/<component_name>.<hpp/cpp>`

Dependencies are bundled into `gen.dep.<hpp/cpp>`.  
Just like the `gen.<hpp/cpp>` they include their components: `dependencies/<dependency_name>.<hpp/cpp>`

The fle processors are in their own respective files. (Ex: `file_processors/<file_processor>.<hpp/cpp>` )  
They directly include `depedencies/file_handling.<hpp/cpp>` as the core library does not include file processing by defualt.

**TODO : Right now the library is not finished, as such the first self-hosting iteration is still WIP**  
Both libraries use *pre-generated* (self-hosting I guess) version of the library to then generate the latest version of itself.  
(sort of a verification that the generated version is equivalent).

The default `gen.bootstrap.cpp` located in the project folder is meant to be produce a standard segmented library, where the components of the library  
have relatively dedicated header and source files. Dependencies included at the top of the file and each header starting with a pragma once.  
The output will be in the `project/gen` directory (if the directory does not exist, it will create it).

Use those to get a general idea of how to make your own tailored version.

Feature Macros:

* `GEN_DEFINE_ATTRIBUTE_TOKENS` : Allows user to define their own attribute macros for use in parsing. 
  * This is auto-generated if using the bootstrap or single-header generation
  * *Note: The user will use the `AttributeTokens.csv` when the library is fully self-hosting.*
* `GEN_DEFINE_LIBRARY_CORE_CONSTANTS` : Optional typename codes as they are non-standard to C/C++ and not necessary to library usage
* `GEN_DONT_USE_NAMESPACE` : By default, the library is wrapped in a `gen` namespace, this will disable that expose it to the global scope.
* `GEN_DONT_ENFORCE_GEN_TIME_GUARD` : By default, the library ( gen.hpp/ gen.cpp ) expects the macro `GEN_TIME` to be defined, this disables that.
* `GEN_ENFORCE_STRONG_CODE_TYPES` : Enforces casts to filtered code types.
* `GEN_EXPOSE_BACKEND` : Will expose symbols meant for internal use only.
* `GEN_ROLL_OWN_DEPENDENCIES` : Optional override so that user may define the dependencies themselves.

## On multi-threading

Currently unsupported. The following changes would have to be made:

## Extending the library

This library is relatively very small, and can be extended without much hassle.

The convention you'll see used throughout the API of the library is as follows:

1. Check name or parameters to make sure they are valid for the construction requested
2. Create a code object using `make_code`.
3. Populate immediate fields (Name, Type, ModuleFlags, etc)
4. Populate sub-entires using `add_entry`. If using the default serialization function `to_string`, follow the order at which entires are expected to appear (there is a strong ordering expected).

Names or Content fields are interned strings and thus showed be cached using `get_cached_string` if its desired to preserve that behavior.

`def_operator` is the most sophisticated constructor as it has multiple permutations of definitions that could be created that are not trivial to determine if valid.

The library has its code segmented into component files, use it to help create a derived version without needing to have to rewrite a generated file directly or build on top of the header via composition or inheritance.
When the scanner is implemented, this will be even easier to customize.
