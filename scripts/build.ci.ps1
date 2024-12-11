# This build script was written to build on windows, however I did setup some generalization to allow for cross platform building.
# It will most likely need a partial rewrite to segment the build process into separate script invocations based on the OS.
# That or just rewrite it in an sh script and call it a day.

$devshell           = Join-Path $PSScriptRoot 'helpers/devshell.ps1'
$misc               = Join-Path $PSScriptRoot 'helpers/misc.psm1'
$refactor_unreal    = Join-Path $PSScriptRoot 'refactor_unreal.ps1'
$incremental_checks = Join-Path $PSScriptRoot 'helpers/incremental_checks.ps1'
$vendor_toolchain   = Join-Path $PSScriptRoot 'helpers/vendor_toolchain.ps1'

Import-Module $misc

$path_root = Get-ScriptRepoRoot

Push-Location $path_root

#region Arguments
       $vendor       = $null
       $release      = $null
	   $verbose      = $false
	   $base         = $false
[bool] $segmented    = $false
[bool] $singleheader = $false
[bool] $c_library    = $false
[bool] $unreal       = $false
[bool] $test         = $false

[array] $vendors = @( "clang", "msvc" )

# This is a really lazy way of parsing the args, could use actual params down the line...

if ( $args ) { $args | ForEach-Object {
	switch ($_){
		{ $_ -in $vendors }   { $vendor       = $_; break }
		"verbose"			  { $verbose      = $true }
		"release"             { $release      = $true }
		"debug"               { $release      = $false }
		"base"                { $base         = $true }
		"segmented"          { $segmented     = $true }
		"singleheader"        { $singleheader = $true }
		"c_library"           { $c_library    = $true }
		"unreal"              { $unreal       = $true }
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
elseif ( $release -eq $false ) {
	$debug = $true
}
else {
	$optimize = $true
}

$cannot_build =                     $base         -eq $false
$cannot_build = $cannot_build -and  $segmented    -eq $false
$cannot_build = $cannot_build -and  $singleheader -eq $false
$cannot_build = $cannot_build -and  $c_library    -eq $false
$cannot_build = $cannot_build -and  $unreal       -eq $false
$cannot_build = $cannot_build -and  $test         -eq $false
if ( $cannot_build ) {
	throw "No build target specified. One must be specified, this script will not assume one"
}

. $vendor_toolchain
. $incremental_checks

write-host "Building gencpp with $vendor"
write-host "Build Type: $(if ($release) {"Release"} else {"Debug"} )"

#region Building
$path_build        = Join-Path $path_root build
$path_base         = Join-Path $path_root base
$path_c_library    = join-Path $path_root gen_c_library
$path_segmented    = Join-Path $path_root gen_segmented
$path_singleheader = Join-Path $path_root gen_singleheader
$path_unreal       = Join-Path $path_root gen_unreal_engine
$path_test         = Join-Path $path_root test
$path_scripts      = Join-Path $path_root scripts

if ( $base )
{
	$path_build    = join-path $path_base      build
	$path_comp     = join-path $path_segmented 'components'
	$path_comp_gen = join-path $path_comp      'gen'

	if ( -not(Test-Path($path_build) )) {
		New-Item -ItemType Directory -Path $path_build
	}
	if ( -not(Test-Path($path_comp_gen) )) {
		New-Item -ItemType Directory -Path $path_comp_gen
	}

	$compiler_args = @()
	$compiler_args += ( $flag_define + 'GEN_TIME' )

	$linker_args   = @(
		$flag_link_win_subsystem_console
	)

	$includes   = @( $path_base)
	$unit       = join-path $path_base  "base.cpp"
	$executable = join-path $path_build "base.exe"

	$result = build-simple $path_build $includes $compiler_args $linker_args $unit $executable

	Push-Location $path_base
		if ( Test-Path( $executable ) ) {
			write-host "`nRunning base"
			$time_taken = Measure-Command { & $executable
					| ForEach-Object {
						write-host `t $_ -ForegroundColor Green
					}
				}
			write-host "`nbase completed in $($time_taken.TotalMilliseconds) ms"
		}
	Pop-Location
}

if ( $segmented )
{
	$path_build = join-path $path_segmented build
	$path_gen   = join-path $path_segmented gen

	if ( -not(Test-Path($path_build) )) {
		New-Item -ItemType Directory -Path $path_build
	}
	if ( -not(Test-Path($path_gen) )) {
		New-Item -ItemType Directory -Path $path_gen
	}

	$compiler_args = @()
	$compiler_args += ( $flag_define + 'GEN_TIME' )

	$linker_args   = @(
		$flag_link_win_subsystem_console
	)

	$includes   = @( $path_base)
	$unit       = join-path $path_segmented "segmented.cpp"
	$executable = join-path $path_build     "segmented.exe"

	$result = build-simple $path_build $includes $compiler_args $linker_args $unit $executable

	Push-Location $path_segmented
		if ( Test-Path( $executable ) ) {
			write-host "`nRunning segmented"
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

	$includes    = @( $path_base )
	$unit       = join-path $path_singleheader "singleheader.cpp"
	$executable = join-path $path_build        "singleheader.exe"

	$compiler_args = @()
	$compiler_args += ( $flag_define + 'GEN_TIME' )

	$linker_args   = @(
		$flag_link_win_subsystem_console
	)

	$result = build-simple $path_build $includes $compiler_args $linker_args $unit $executable

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

if ( $c_library )
{
	$path_build = join-path $path_c_library build
	$path_gen   = join-path $path_c_library gen

	if ( -not(Test-Path($path_build) )) {
		New-Item -ItemType Directory -Path $path_build
	}
	if ( -not(Test-Path($path_gen) )) {
		New-Item -ItemType Directory -Path $path_gen
	}

	$includes    = @( $path_base )
	$unit       = join-path $path_c_library "c_library.cpp"
	$executable = join-path $path_build     "c_library.exe"

	$compiler_args = @()
	$compiler_args += ( $flag_define + 'GEN_TIME' )

	$linker_args   = @(
		$flag_link_win_subsystem_console
	)

	$result = build-simple $path_build $includes $compiler_args $linker_args $unit $executable

	Push-Location $path_c_library
		if ( Test-Path( $executable ) ) {
			write-host "`nRunning c_library generator"
			$time_taken = Measure-Command { & $executable
					| ForEach-Object {
						write-host `t $_ -ForegroundColor Green
					}
				}
			write-host "`nc_library generator completed in $($time_taken.TotalMilliseconds) ms"
		}
	Pop-Location

	$unit       = join-path $path_c_library "gen.c"
	$executable = join-path $path_build     "gen_c_library_test.exe"

	if ($vendor -eq "clang") {
		$compiler_args += '-x'
		$compiler_args += 'c'
		$compiler_args += '-std=c11'
	} elseif ($vendor -eq "msvc") {
		$compiler_args += "/TC"       # Compile as C
		$compiler_args += "/Zc:__cplusplus" # Fix __cplusplus macro
		$compiler_args += "/std:c11"
	}

	$result = build-simple $path_build $includes $compiler_args $linker_args $unit $executable

	Push-Location $path_c_library
		if ( Test-Path( $executable ) ) {
			write-host "`nRunning c_library test"
			$time_taken = Measure-Command { & $executable
					| ForEach-Object {
						write-host `t $_ -ForegroundColor Green
					}
				}
			write-host "`nc_library generator completed in $($time_taken.TotalMilliseconds) ms"
		}
	Pop-Location
}

if ( $unreal )
{
	$path_build = join-path $path_unreal build
	$path_gen   = join-path $path_unreal gen

	if ( -not(Test-Path($path_build) )) {
		New-Item -ItemType Directory -Path $path_build
	}
	if ( -not(Test-Path($path_gen) )) {
		New-Item -ItemType Directory -Path $path_gen
	}

	$includes    = @( $path_base )
	$unit       = join-path $path_unreal "unreal.cpp"
	$executable = join-path $path_build  "unreal.exe"

	$compiler_args = @()
	$compiler_args += ( $flag_define + 'GEN_TIME' )

	$linker_args   = @(
		$flag_link_win_subsystem_console
	)

	$result = build-simple $path_build $includes $compiler_args $linker_args $unit $executable

	Push-Location $path_unreal
		if ( Test-Path( $executable ) ) {
			write-host "`nRunning unreal variant generator"
			$time_taken = Measure-Command { & $executable
					| ForEach-Object {
						write-host `t $_ -ForegroundColor Green
					}
				}
			write-host "`n Unreal variant generator completed in $($time_taken.TotalMilliseconds) ms"
		}
	Pop-Location

	. $refactor_unreal
}

# TODO(Ed): The unit testing needs a full rewrite
if ( $test -and $false )
{
	$path_gen          = join-path $path_test gen
	$path_gen_build    = join-path $path_gen  build
	$path_build        = join-path $path_test build
	$path_original     = join-path $path_gen  original
	$path_components   = join-path $path_original components
	$path_dependencies = join-path $path_original dependencies
	$path_helpers      = join-path $path_original helpers

	if ( -not(Test-Path($path_build) )) {
		New-Item -ItemType Directory -Path $path_build
	}
	if ( -not(Test-Path($path_gen) )) {
		New-Item -ItemType Directory -Path $path_gen
	}
	if ( -not(Test-Path($path_gen_build) ))  {
		New-Item -ItemType Directory -Path $path_gen_build
	}
	if ( -not(test-path $path_original)) {
		new-item -ItemType Directory -Path $path_original
	}
	if ( -not(test-path $path_components)) {
		new-item -ItemType Directory -Path $path_components
	}
	if ( -not(test-path $path_dependencies)) {
		new-item -ItemType Directory -Path $path_dependencies
	}
	if ( -not(test-path $path_helpers)) {
		new-item -ItemType Directory -Path $path_helpers
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

	$result = build-simple $path_build $includes $compiler_args $linker_args $unit $executable

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

Pop-Location # $path_root
