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

write-host "`n`nBuilding gencpp bootstrap`n"

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
