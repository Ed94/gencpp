## Navigation

# base

[Top](../Readme.md)

* [docs](../docs/Readme.md)

# C Library Generation

`c_library.cpp` generates both *segemnted* and *singleheader* variants of the library compliant with C11.

The output will be in the `gen_segmented/gen` directory (if the directory does not exist, it will create it).

If using the library's provided build scripts:

```ps1
.\build.ps1 <compiler> <debug or omit> c_library
```

All free from tag identifiers will be prefixed with `gen_` or `GEN_` as the namespace. This can either be changed after generation with a `.refactor` script (or your preferred subst method), OR by modifying c_library.refactor.

**If c_library.refactor is modified you may need to modify c_library.cpp and its [components](./components/). As some of the container generation relies on that prefix.**
