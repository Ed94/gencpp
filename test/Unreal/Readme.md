# Unreal Header & Source reconstruction tests

***Note: This validation test has not been implemented yet.***

Will test the following modules + plugins:

* Kismet
* Slate
* RTTI Bases
* Gameframework
* Actor & Component Bases
* Lyra

In the future I could attempt to do a similar test to that of the godot engine full compilation test.

For now it just does the following:

* Download the Unreal source code
* For each module
    1. Grab all header and source file paths
    2. Generate an ast for each file and serialize it to a file called `<name of file>.gen.<h/c>`
    3. Reconstruct the ast from the generated file
    4. Compare the original ast to the reconstructed ast

This wil most likely be the most difficult test along-side godot's full compilation test.
