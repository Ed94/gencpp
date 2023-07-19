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
