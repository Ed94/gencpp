# Generate Segemented Library

The principal (user) files are `gen.hpp` and `gen.cpp`.
They contain includes for its various components: `components/<component_name>.<hpp/cpp>`

Dependencies are bundled into `gen.dep.<hpp/cpp>`. They are included in `gen.<hpp/cpp>` before component includes.
Just like the `gen.<hpp/cpp>` they include their components: `dependencies/<dependency_name>.<hpp/cpp>`


Both libraries use *pre-generated* (self-hosting I guess) version of the library to then generate the latest version of itself.

They dedicated header and source files. Dependencies included at the top of the file and each header starting with a pragma once.
The output will be in the `project/gen` directory (if the directory does not exist, it will create it).

Use those to get a general idea of how to make your own tailored version.
