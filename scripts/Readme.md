# Scripts

Generation, testing, and cleanup scripts for the test directory are found here along with `natvis` and `natstepfilter` files for debugging.

## Refactoring

`refactor.ps1` Provides a way to run the [refactor](github.com/Ed94/refactor) program. It uses the `gencpp.refactor` script to complete a mass refactor of all content within the files of the specified within the script.

Currently `refactor` only supports naive sort of *find and replace* feature set and will not be able to rename identifiers excluisvely to a specific context (such as only renaming member names of a specific struct, etc).

## Build & Run Scripts

**`clean.ps1`**  
Remove any generated content from the repository.

**`bootstrap.ps1`**  
Generate a version of gencpp where components are inlined directly to `gen.<hpp/cpp>` and `gen. <hpp/cpp>`  
Any heavily preprocessed code is not inlined and are instead generated using the code in the `helpers` directory.

**`singlheader.build.ps1`**  
Generate a single-header version of the library where all code that would normally good in the usual four files (see bootstrap) are inlined into a single `gen.hpp` file.  
As with the bootstrap, any heavily preprocessed code is not inlined and instead generated with helper code.

**`test.gen.build.ps1`**  
Build the metaprogram for generating the test code.

**`test.gen.ps1`**  
Build (if not already) the metaprogram for generating test code, then run it to generate code.

**`test.build.ps1`**  
Build and run metaprogram, build test program.

**`test.run.ps1`**  
Build and run metaprogram, build and run test program.
