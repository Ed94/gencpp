# This build script was written to build on windows, however I did setup some generalization to allow for cross platform building.
# It will most likely need a partial rewrite to segment the build process into separate script invocations based on the OS.
# That or just rewrite it in an sh script and call it a day.

Import-Module ./helpers/target_arch.psm1
$target_arch        = Join-Path $PSScriptRoot 'helpers/target_arch.psm1'
$devshell           = Join-Path $PSScriptRoot 'helpers/devshell.ps1'
$format_cpp	        = Join-Path $PSScriptRoot 'helpers/format_cpp.psm1'
$incremental_checks = Join-Path $PSScriptRoot 'helpers/incremental_checks.ps1'
$vendor_toolchain   = Join-Path $PSScriptRoot 'helpers/vendor_toolchain.ps1'

$path_root = git rev-parse --show-toplevel

Import-Module $target_arch
Import-Module $format_cpp

Push-Location $path_root

#region Arguments
       $vendor       = $null
       $release      = $null
	   $verbose      = $false
[bool] $bootstrap    = $false
[bool] $singleheader = $false
[bool] $test         = $false

[array] $vendors = @( "clang", "msvc" )

# This is a really lazy way of parsing the args, could use actual params down the line...

if ( $args ) { $args | ForEach-Object {
	switch ($_){
		{ $_ -in $vendors }   { $vendor       = $_; break }
		"verbose"			  { $verbose      = $true }
		"release"             { $release      = $true }
		"debug"               { $release      = $false }
		"bootstrap"           { $bootstrap    = $true }
		"singleheader"        { $singleheader = $true }
		"test"                { $test         = $true }
	}
}}
#endregion Arguments

#region Configuration
if ($IsWindows) {
	# This library was really designed to only run on 64-bit systems.
	# (Its a development tool after all)
    & $devshell -arch amd64
}

if ( $vendor -eq $null ) {
	write-host "No vendor specified, assuming clang available"
	$compiler = "clang"
}

if ( $release -eq $null ) {
	write-host "No build type specified, assuming debug"
	$release = $false
}

if ( $bootstrap -eq $false -and $singleheader -eq $false -and $test -eq $false ) {
	throw "No build target specified. One must be specified, this script will not assume one"
}

. $vendor_toolchain
. $incremental_checks

write-host "Building gencpp with $vendor"
write-host "Build Type: $(if ($release) {"Release"} else {"Debug"} )"

#region Building
$path_build        = Join-Path $path_root build
$path_project      = Join-Path $path_root project
$path_scripts      = Join-Path $path_root scripts
$path_singleheader = Join-Path $path_root singleheader
$path_test         = Join-Path $path_root test

if ( $bootstrap )
{
	$path_build    = join-path $path_project build
	$path_gen      = join-path $path_project gen
	$path_comp_gen = join-path $path_project components/gen

	if ( -not(Test-Path($path_build) )) {
		New-Item -ItemType Directory -Path $path_build
	}
	if ( -not(Test-Path($path_gen) )) {
		New-Item -ItemType Directory -Path $path_gen
	}
	if ( -not(Test-Path($path_comp_gen) )) {
		New-Item -ItemType Directory -Path $path_comp_gen
	}

	$compiler_args = @()
	$compiler_args += ( $flag_define + 'GEN_TIME' )

	$linker_args   = @(
		$flag_link_win_subsystem_console
	)

	$includes   = @( $path_project)
	$unit       = join-path $path_project "bootstrap.cpp"
	$executable = join-path $path_build   "bootstrap.exe"

	build-simple $path_build $includes $compiler_args $linker_args $unit $executable

	Push-Location $path_project
		if ( Test-Path( $executable ) ) {
			write-host "`nRunning bootstrap"
			$time_taken = Measure-Command { & $executable
					| ForEach-Object {
						write-host `t $_ -ForegroundColor Green
					}
				}
			write-host "`nBootstrap completed in $($time_taken.TotalMilliseconds) ms"
		}
	Pop-Location
}

if ( $singleheader )
{
	$path_build = join-path $path_singleheader build
	$path_gen   = join-path $path_singleheader gen

	if ( -not(Test-Path($path_build) )) {
		New-Item -ItemType Directory -Path $path_build
	}
	if ( -not(Test-Path($path_gen) )) {
		New-Item -ItemType Directory -Path $path_gen
	}

	$includes    = @( $path_project )
	$unit       = join-path $path_singleheader "singleheader.cpp"
	$executable = join-path $path_build        "singleheader.exe"

	$compiler_args = @()
	$compiler_args += ( $flag_define + 'GEN_TIME' )

	$linker_args   = @(
		$flag_link_win_subsystem_console
	)

	build-simple $path_build $includes $compiler_args $linker_args $unit $executable

	Push-Location $path_singleheader
		if ( Test-Path( $executable ) ) {
			write-host "`nRunning singleheader generator"
			$time_taken = Measure-Command { & $executable
					| ForEach-Object {
						write-host `t $_ -ForegroundColor Green
					}
				}
			write-host "`nSingleheader generator completed in $($time_taken.TotalMilliseconds) ms"
		}
	Pop-Location
}

if ( $test )
{
	$path_gen       = join-path $path_test gen
	$path_gen_build = join-path $path_gen  build
	$path_build     = join-path $path_test build

	if ( -not(Test-Path($path_build) )) {
		New-Item -ItemType Directory -Path $path_build
	}
	if ( -not(Test-Path($path_gen) )) {
		New-Item -ItemType Directory -Path $path_gen
	}
	if ( -not(Test-Path($path_gen_build) ))  {
		New-Item -ItemType Directory -Path $path_gen_build
	}

	$path_bootstrap = join-path $path_project gen

	$includes    = @( $path_bootstrap )
	$unit       = join-path $path_test  "test.cpp"
	$executable = join-path $path_build "test.exe"

	$compiler_args = @()
	$compiler_args += ( $flag_define + 'GEN_TIME' )

	$linker_args   = @(
		$flag_link_win_subsystem_console
	)

	build-simple $path_build $includes $compiler_args $linker_args $unit $executable

	Push-Location $path_test
		Write-Host $path_test
		if ( Test-Path( $executable ) ) {
			write-host "`nRunning test generator"
			$time_taken = Measure-Command { & $executable
					| ForEach-Object {
						write-host `t $_ -ForegroundColor Green
					}
				}
			write-host "`nTest generator completed in $($time_taken.TotalMilliseconds) ms"
		}
	Pop-Location
}
#endregion Building

#region Formatting
push-location $path_scripts
if ( $bootstrap -and (Test-Path (Join-Path $path_project "gen/gen.hpp")) )
{
	$path_gen      = join-path $path_project gen
	$path_comp_gen = join-path $path_project components/gen
	$include  = @(
		'gen.hpp', 'gen.cpp',
		'gen.dep.hpp', 'gen.dep.cpp',
		'gen.builder.hpp', 'gen.builder.cpp'
		'gen.scanner.hpp', 'gen.scanner.cpp'
	)
	$exclude  = $null
	format-cpp $path_gen $include $exclude
	format-cpp $path_comp_gen @( 'ast_inlines.hpp', 'ecode.hpp', 'especifier.hpp', 'eoperator.hpp', 'etoktype.cpp' ) $null

}

if ( $singleheader -and (Test-Path (Join-Path $path_singleheader "gen/gen.hpp")) )
{
	$path_gen = join-path $path_singleheader gen
	$include  = @(
		'gen.hpp'
	)
	$exclude  = $null
	format-cpp $path_gen $include $exclude
}

if ( $test -and $false )
{
	$path_gen = join-path $path_test gen
	$include  = @(
		'*.gen.hpp'
	)
	$exclude  = $null
	format-cpp $path_gen $include $exclude
}
pop-location
#endregion Formatting

Pop-Location # $path_root
