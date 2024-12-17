## Navigation

# base

[Top](../Readme.md)

* [docs](../docs/Readme.md)

# segmented Library Generation

Create a segmented library using `segmented.cpp`

The principal (user) files are `gen.hpp` and `gen.cpp`.
They contain includes for its various components: `components/<component_name>.<hpp/cpp>`

Dependencies are bundled into `gen.dep.<hpp/cpp>`. They are included in `gen.<hpp/cpp>` before component includes.
Just like the `gen.<hpp/cpp>` they include their components: `dependencies/<dependency_name>.<hpp/cpp>`. The auxiliary content (builder & scanner) is given their own files.

If using the library's provided build scripts:

```ps1
.\build.ps1 <compiler> <debug or omit> segmented
```
