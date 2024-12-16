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
[bool] $verbose      = $false
[bool] $base         = $false
[bool] $segmented    = $false
[bool] $singleheader = $false
[bool] $c_lib        = $false
[bool] $c_lib_static = $false
[bool] $c_lib_dyn    = $false
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
		"segmented"           { $segmented    = $true }
		"singleheader"        { $singleheader = $true }
		"c_lib"               { $c_lib        = $true }
		"c_lib_static"        { $c_lib_static = $true }
		"c_lib_dyn"           { $c_lib_dyn    = $true }
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
$cannot_build = $cannot_build -and  $c_lib        -eq $false
$cannot_build = $cannot_build -and  $c_lib_static -eq $false
$cannot_build = $cannot_build -and  $c_lib_dyn    -eq $false
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

if ( $c_lib -or $c_lib_static -or $c_lib_dyn )
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
}

if ( $c_lib_static )
{
	$includes = @( $path_c_library )
	$unit     = join-path $path_c_library "gen_c_lib.c"
	$path_lib = join-path $path_build     "gencpp_c11.lib"

	$compiler_args = @()
	$compiler_args += $flag_all_c
	$compiler_args += $flag_updated_cpp_macro
	$compiler_args += $flag_c11

	$linker_args = @()
	$result = build-simple $path_build $includes $compiler_args $linker_args $unit $path_lib
}

if ( $c_lib_dyn )
{
	$includes = @( $path_c_library )
	$unit     = join-path $path_c_library "gen_c_lib.c"
	$path_dll = join-path $path_build     "gencpp_c11.dll"
 
	$compiler_args = @()
	$compiler_args += $flag_all_c
	$compiler_args += $flag_updated_cpp_macro
	$compiler_args += $flag_c11
	$compiler_args += ( $flag_define + 'GEN_DYN_LINK' )
	$compiler_args += ( $flag_define + 'GEN_DYN_EXPORT' )
 
	$linker_args = @()
	$result = build-simple $path_build $includes $compiler_args $linker_args $unit $path_dll
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

# C Library testing
if ( $test )
{
	$path_test_c = join-path $path_test   c_library
	$path_build  = join-path $path_test_c build
	$path_gen    = join-path $path_test_c gen
	if ( -not(Test-Path($path_build) )) {
		New-Item -ItemType Directory -Path $path_build
	}
	if ( -not(Test-Path($path_gen) )) {
		New-Item -ItemType Directory -Path $path_gen
	}

	$path_singleheader_include = join-path $path_c_library gen
	$includes    = @( $path_singleheader_include )
	$unit       = join-path $path_test_c "test.c"
	$executable = join-path $path_build  "test.exe"

	$compiler_args = @()
	$compiler_args += ( $flag_define + 'GEN_TIME' )
	$compiler_args += $flag_all_c
	$compiler_args += $flag_updated_cpp_macro
	$compiler_args += $flag_c11

	$linker_args   = @(
		$flag_link_win_subsystem_console
	)

	$result = build-simple $path_build $includes $compiler_args $linker_args $unit $executable

	Push-Location $path_test_c
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

if ($test)
{
	$path_test_cpp = join-path $path_test     cpp_library
	$path_build    = join-path $path_test_cpp build
	$path_gen      = join-path $path_test_cpp gen
	if ( -not(Test-Path($path_build) )) {
		New-Item -ItemType Directory -Path $path_build
	}
	if ( -not(Test-Path($path_gen) )) {
		New-Item -ItemType Directory -Path $path_gen
	}

	$path_singleheader_include = join-path $path_singleheader gen
	$includes    = @( $path_singleheader_include )
	$unit       = join-path $path_test_cpp "test.cpp"
	$executable = join-path $path_build    "test.exe"

	$compiler_args = @()
	$compiler_args += ( $flag_define + 'GEN_TIME' )

	$linker_args   = @(
		$flag_link_win_subsystem_console
	)

	$result = build-simple $path_build $includes $compiler_args $linker_args $unit $executable

	Push-Location $path_test_cpp
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
#endregion Building

Pop-Location # $path_root
