cls

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

$path_root       = git rev-parse --show-toplevel
$path_build      = Join-Path $path_root build
$path_scripts    = Join-Path $path_root scripts
$path_test       = Join-Path $path_root test
$path_gen        = Join-Path $path_test gen
$path_test_build = Join-Path $path_test build
$path_gen_build  = Join-Path $path_gen  build

write-host "`n`nBuilding Test`n"

if ( -not( Test-Path $path_gen_build ) )
{
# Generate build files for meta-program
Push-Location $path_gen
	$args_meson  = @()
	$args_meson += "setup"
	$args_meson += $path_gen_build

	& meson $args_meson
Pop-Location
}

# Compile meta-program
Push-Location $path_root
	$args_ninja = @()
	$args_ninja += "-C"
	$args_ninja += $path_gen_build

	& ninja $args_ninja
Pop-Location

Push-location $path_gen
# Run meta-program
	$gencpp = Join-Path $path_gen_build gencpp.exe

	Write-Host `nRunning tests...
	& $gencpp

# Format generated files
	$path_clang_format = Join-Path $path_scripts .clang-format

	Write-Host `nBeginning format...
	$formatParams = @(
		'-i'          # In-place
		"-style=file:$path_clang_format" # Search for a .clang-format file in the parent directory of the source file.
		'-verbose'
	)

	$include = @('*.gen.hpp', '*.gen.cpp')
	$exclude = $null

	$targetFiles = @(Get-ChildItem -Recurse -Path $path_gen -Include $include -Exclude $exclude | Select-Object -ExpandProperty FullName)

	clang-format $formatParams $targetFiles
	Write-Host "`nFormatting complete"
Pop-Location

# Build the program depending on generated files.
if ( -not( Test-Path $path_test_build ) )
{
Push-Location $path_test
	$args_meson = @()
	$args_meson += "setup"
	$args_meson += $path_test_build

# 	& meson $args_meson
Pop-Location
}

Push-Location $path_root
	$args_ninja = @()
	$args_ninja += "-C"
	$args_ninja += $path_test_build

	# ninja $args_ninja
Pop-Location

Push-Location $path_test
	$testcpp = Join-Path $path_test_build testcpp.exe

	# & $testcpp
Pop-Location
