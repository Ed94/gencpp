# Scripts

Generation, testing, and cleanup scripts for the test directory are found here along with `natvis` and `natstepfilter` files for debugging.

## Refactoring

`refactor.ps1` Provides a way to run the [refactor](github.com/Ed94/refactor) program. It uses the `gencpp.refactor` script to complete a mass refactor of all content within the files of the specified within the script.

Currently `refactor` only supports naive sort of *find and replace* feature set and will not be able to rename identifiers excluisvely to a specific context (such as only renaming member names of a specific struct, etc).

**Note: The following macros are used with specifiers and token parsing within the library:**

* global
* internal
* local_persist
* forceinline
* neverinline

IF they are changed the following files would need adjustment:

* `./project/enums/ESpecifier.csv`
* `./project/enums/ETokType.csv`
* `./project/helpers/helper.hpp`

## Build & Run Scripts

**`clean.ps1`**  
Remove any generated content from the repository.

**`build.ps1`**  
Build bootstrap, singleheader, or tests. Supports MSVC or clang, release or debug.

```
args:
    bootstrap
    singleheader
    test
    clang
    msvc    : By default this project builds with clang, specifying msvc will build with MSVC.
    debug
    release : By default this project builds in debug mode, specifying release will build with optimizations.
```

**`package_release.ps1`**  
Will build the project as fast as possible, then package the release into a zip file.

*Note: My env is Windows 11 with MSVC 2022 and clang 16.0.6*
