# target_arch.psm1

function Get-TargetArchClang {
    # Get the target architecture by querying clang itself
    $output = & clang -v 2>&1
    foreach ($line in $output) {
        if ($line -like "*Target:*") {
            $clangTarget = ($line -split ':')[1].Trim()
            return $clangTarget
        }
    }
    throw "Clang target architecture could not be determined."
}

function Get-TargetArchMSVC {
    # Assuming you've set the Visual Studio environment variables using `vcvarsall.bat`
    # This looks for the `VSCMD_ARG_TGT_ARCH` environment variable which Visual Studio sets to indicate the target architecture.
    $arch = $env:VSCMD_ARG_TGT_ARCH
    if (-not $arch) {
        throw "MSVC target architecture could not be determined. Ensure you've initialized the Visual Studio environment."
    }
    return $arch
}

Export-ModuleMember -Function Get-TargetArchClang, Get-TargetArchMSVC
