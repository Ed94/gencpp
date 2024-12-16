## Navigation

# base

[Top](../Readme.md)

* [docs](../docs/Readme.md)

# Unreal Engine Version Generator

This generates a variant of gencpp thats compatiable with use as a thirdparty module within a plugin or module of an Unreal Project or the Engine itself.

If using the library's provided build scripts:

```ps1
.\build.ps1 <compiler> <debug or omit> unreal
```

## Notables

For the most part this follows the same conventions as `gen_segmented`.

This generator uses a separate enumeration definitions for the following:

* [AttributeTokens.csv](./enums/AttributeTokens.csv) : Add your own <MODULE>_API attributes, etc here that are encountered within the Engine.
* [ESpecifier.csv](./enums/ESpecifier.csv) : Adds the `FORCEINLINE` & `FORCEINLINE_DEBUGGABLE` specfiers (additions are made as they are encountered)
* [ETokType.csv](./enums/ETokType.csv) : Same modifications as ESpecifier.csv.

A separate [parser_case_macros.cpp](./components/parser_case_macros.cpp) is used to accomodate for the new forceinline specifiers.

The `global` macro the library uses is redefined as an empty substiution.


The expected usage of this library is to put into into a third-party plugin module to then use either in editor modules or in shell script done in some stage of hot-reloading or building the Unreal Engine or Project.
