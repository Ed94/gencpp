# Test : Godot full AST reconstruction and compile validation

***Note: This validation test has not been implemented yet.***

Repo : https://github.com/godotengine/godot

* Download the Unreal source code
* Find paths of every header and source file
* Generate an ast for each file and serialize it to a file called `<name of file>.gen.<h/c>`
* Reconstruct the ast from the generated file
* Compare the original ast to the reconstructed ast
* If all ASTs are considered valid, overwrite the original files with the generated files
* Compile the engine.

Currently the most involved test planned for the library.
