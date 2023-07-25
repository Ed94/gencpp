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

Use those to get a general idea of how to make your own tailored version.

If the naming convention is undesired, the `gencpp.refactor` script can be used with the [refactor]()

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
