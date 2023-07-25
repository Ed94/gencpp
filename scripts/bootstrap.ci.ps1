[string] $type = $null
[string] $test = $false

foreach ( $arg in $args )
{
	if ( $arg -eq "test" )
	{
		$test = $true
	}
	else
	{
		$type = $arg
	}
}

$path_root            = git rev-parse --show-toplevel
$path_project         = Join-Path $path_root project
$path_project_build   = Join-Path $path_project build
$path_project_gen     = Join-Path $path_project gen

write-host "`n`nBuilding gencpp bootstrap`n"

if ( -not( Test-Path $path_project_build) )
{
# Generate build files for meta-program
Push-Location $path_project
	$args_meson  = @()
	$args_meson += "setup"
	$args_meson += $path_project_build

	& meson $args_meson
Pop-Location
}

# Compile meta-program
Push-Location $path_root
	$args_ninja = @()
	$args_ninja += "-C"
	$args_ninja += $path_project_build

	& ninja $args_ninja
Pop-Location

Push-location $path_project
if ( -not(Test-Path($path_project_gen) )) {
	New-Item -ItemType Directory -Path $path_project_gen
}

# Run meta-program
$gencpp_bootstrap = Join-Path $path_project_build gencpp_bootstrap.exe

Write-Host `nRunning gencpp bootstrap...
& $gencpp_bootstrap

# Format generated files
Write-Host `nBeginning format...
$formatParams = @(
	'-i'          # In-place
	'-style=file:../scripts/.clang-format'
	'-verbose'
)

$include = @('gen.hpp', 'gen.cpp', 'gen_dep.hpp', 'gen_dep.cpp')
$exclude = $null

$targetFiles = @(Get-ChildItem -Recurse -Path $path_project_gen -Include $include -Exclude $exclude | Select-Object -ExpandProperty FullName)

clang-format $formatParams $targetFiles
Write-Host "`nFormatting complete"
Pop-Location
