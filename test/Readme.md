# Baseline verification

The implementation smoke is `verify_baseline.ps1`. It is not a parser corpus.

From the project root, in a fresh PowerShell process:

```powershell
.\scripts\verify_baseline.ps1 -Mode positive -MirrorRoot ..\gencpp-verify-positive
.\scripts\verify_baseline.ps1 -Mode positive -MirrorRoot ..\gencpp-verify-positive -ReuseMirror
```

The first command generates base component tables, segmented C++, single-header C++, and both C11 layouts.
It compiles and runs the C++17 and C11 single-header host smokes. The second command reuses the same mirror.

The verification wrapper copies them into the disposable mirror; it does not embed C or C++ source text.

## Parser bounds probe

`test/parser_bounds/test.cpp` is a GEN_TIME program. It includes `base/gen.cpp`. It is not part of `verify_baseline.ps1`.

From the project root:

```powershell
.\scripts\build.ps1 clang parser_bounds
```

The script builds and runs the probe. A pass prints `parser bounds passed` and `PASSED: parser_bounds`.
The probe covers helper positions, formatting skip at the slice end, empty and whitespace `parse_variable`, and two direct end-of-slice reads.
It does not prove in-slice syntax errors return.

## Lexer failure probe

```powershell
.\scripts\build.ps1 clang lexer_failures
```

A pass prints `lexer failures passed` and `PASSED: lexer_failures`.
The probe covers nine lexer failures, six successful sibling inputs, and a baseline variable after each failure.
It does not prove in-slice parser errors return.
