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

$path_root               = git rev-parse --show-toplevel
$path_singleheader       = Join-Path $path_root singleheader
$path_singleheader_build = Join-Path $path_singleheader build
$path_singleheader_gen   = Join-Path $path_singleheader gen

write-host "`n`nBuilding gencpp singleheader`n"

if ( -not( Test-Path $path_singleheader_build) )
{
# Generate build files for meta-program
Push-Location $path_singleheader
	$args_meson  = @()
	$args_meson += "setup"
	$args_meson += $path_singleheader_build

	& meson $args_meson
Pop-Location
}

# Compile meta-program
Push-Location $path_root
	$args_ninja = @()
	$args_ninja += "-C"
	$args_ninja += $path_singleheader_build

	& ninja $args_ninja
Pop-Location

Push-location $path_singleheader
	if ( -not(Test-Path($path_singleheader_gen) )) {
		New-Item -ItemType Directory -Path $path_singleheader_gen
	}

	# Run meta-program
	$gencpp_singleheader = Join-Path $path_singleheader_build gencpp_singleheader.exe

	Write-Host `nRunning gencpp singleheader...
	& $gencpp_singleheader

	# Format generated files
	Write-Host `nBeginning format...
	$formatParams = @(
		'-i'          # In-place
		'-style=file:../scripts/.clang-format'
		'-verbose'
	)

	$include = @('gen.hpp')
	$exclude = $null

	$targetFiles = @(Get-ChildItem -Recurse -Path $path_project -Include $include -Exclude $exclude | Select-Object -ExpandProperty FullName)

	clang-format $formatParams $targetFiles
	Write-Host "`nFormatting complete"
Pop-Location

# Build and run validation

