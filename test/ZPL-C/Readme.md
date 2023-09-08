# Test : ZPL-C Reconstruction

***Note: This validation test has not been implemented yet.***

Repo : https://github.com/zpl-c/zpl

This is a AST reconstruction test of the ZPL-C library.

Much of the dependency code used in gencpp is derived from the ZPL-C library.
In the future I could directly generate that related code from the ZPL-C library.

For now it just does the following:

1. Download the ZPL-C library
2. Grab all header and source file paths
3. Generate an ast for each file and serialize it to a file called <name of file>.gen.<h/c>
4. Reconstruct the ast from the generated file
5. Compare the original ast to the reconstructed ast
