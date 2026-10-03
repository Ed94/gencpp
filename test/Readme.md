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
