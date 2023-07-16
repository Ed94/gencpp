# Scripts

Build and cleanup scripts for the test deirectory are found here along with `natvis` and `natstepfilter` files for debugging.

The build works as follows:

* Compile and run the meta-program, it will dump files to the `test/gen` directory.
* Format the files using clang-format
* Build a program that uses some the generated definitions. (Have not done yet)

The `test/gen` directory has the meson.build config for the meta-program
The `test` directory has the one for the depdendent-program.
