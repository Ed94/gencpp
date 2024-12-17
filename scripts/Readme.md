# Scripts

Generation, testing, and cleanup scripts for the test directory are found here along with `natvis` and `natstepfilter` files for debugging.

## Refactoring

`refactor.ps1` Provides a way to run the [refactor](github.com/Ed94/refactor) program. It uses the a `.refactor` script (such as [`gencpp.refactor`](../base/gencpp.refactor)) to complete a mass refactor of all content within the files of the specified within the script.

Currently `refactor` only supports naive sort of *find and replace* feature set and will not be able to rename identifiers excluisvely to a specific context (such as only renaming member names of a specific struct, etc).  
Its main uage is the [c_library generation](../gen_c_library/).

## Build & Run Scripts

**`clean.ps1`**  
Remove any generated content from the repository.

**`build.ps1`**  
Build c library (segmented, singleheader, static, or dynamic), cpp library (segmented, singleheader, or unreal). Supports msvc or clang, release or debug.

```erlang
args:
    c_lib        : Build c11 library (singleheader & segmented)
    c_lib_static : Build static  c11 library
    c_lib_dyn    : Buidl dyanmic c11
    segmented
    singleheader
    unreal
    clang
    msvc    : By default this project builds with clang, specifying msvc will build with MSVC.
    debug
    release : By default this project builds in debug mode, specifying release will build with optimizations.
```

**`package_release.ps1`**  
Will build the build all, then package the release into a zip file.

*Note: My env is Windows 11 with MSVC 2022 and clang 16.0.6*
