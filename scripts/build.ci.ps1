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


$path_root    = git rev-parse --show-toplevel
$path_build   = Join-Path $path_root build
$path_scripts = Join-Path $path_root scripts


# if ( $test -eq $true )
# {
	#region Test Build
	write-host "`n`nBuilding Test`n"

	$path_test       = Join-Path $path_root test
	$path_gen        = Join-Path $path_test gen
	$path_test_build = Join-Path $path_test build
	$path_gen_build  = Join-Path $path_gen  build

	# Generate files using metaprogram.
	if ( -not( Test-Path $path_gen_build ) )
	{
		$args_meson  = @()
		$args_meson += "setup"
		$args_meson += $path_gen_build

		Push-Location $path_gen
		& meson $args_meson
		Pop-Location
	}

	$args_ninja = @()
	$args_ninja += "-C"
	$args_ninja += $path_gen_build

	Push-Location $path_root
	& ninja $args_ninja
	Pop-Location

	$gencpp = Join-Path $path_gen_build gencpp.exe

	Push-location $path_gen
	& $gencpp
	Pop-Location


	# Build the program depending on generated files.
	# if ( -not( Test-Path $path_test_build ) )
	# {
	# 	$args_meson = @()
	# 	$args_meson += "setup"
	# 	$args_meson += $path_test_build

	# 	Push-Location $path_test
	# 	& meson $args_meson
	# 	Pop-Location
	# }

	# $args_ninja = @()
	# $args_ninja += "-C"
	# $args_ninja += $path_test_build

	# Push-Location $path_root
	# ninja $args_ninja
	# Pop-Location

	# $testcpp = Join-Path $path_test_build testcpp.exe

	# Push-Location $path_test
	# & $testcpp
	# Pop-Location
	# endregion Test Build
# }
