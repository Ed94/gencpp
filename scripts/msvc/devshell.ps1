# This script is used to iniitate the MSVC DevShell
$vs_devshell = @()
@("enterprise", "professional", "community") | ForEach-Object {
    $vs_devshell_2022 = "C:\Program Files\Microsoft Visual Studio\2022\"       + $_ + "\Common7\Tools\Launch-VsDevShell.ps1"
    $vs_devshell_2019 = "C:\Program Files (x86)\Microsoft Visual Studio\2019\" + $_ + "\Common7\Tools\Launch-VsDevShell.ps1"

    $vs_devshell += @( $vs_devshell_2022, $vs_devshell_2019 )
}

$found = $false
foreach($path in $vs_devshell) {
    if (Test-Path $path) {
        write-host "Found $path"

        Push-Location # Save the current path, loading the script will change it.
        & $path
        Pop-Location

        $found = $true
        break;
    }
}

if (-not $found) {
    write-host "MSVC DevShell: No valid path found"
}
