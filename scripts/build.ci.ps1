# This build script was written to build on windows, however I did setup some generalization to allow for cross platform building.
# It will most likely need a partial rewrite to segment the build process into separate script invocations based on the OS.
# That or just rewrite it in an sh script and call it a day.

$devshell           = Join-Path $PSScriptRoot 'helpers/devshell.ps1'
$misc               = Join-Path $PSScriptRoot 'helpers/misc.psm1'
$refactor_unreal    = Join-Path $PSScriptRoot 'refactor_unreal.ps1'
$incremental_checks = Join-Path $PSScriptRoot 'helpers/incremental_checks.ps1'
$vendor_toolchain   = Join-Path $PSScriptRoot 'helpers/vendor_toolchain.ps1'

Import-Module $misc

function stop-stage {
    param([string]$stage, [int]$code_exit = 1)
    Write-Host "FAILED: $stage" -ForegroundColor Red
    exit $code_exit
}

function invoke-stageTool {
    param(
        [string]$stage,
        [bool]$compiled,
        [string]$executable,
        [string]$path_work
    )
    if (-not $compiled) {
        stop-stage "$stage compile/link"
    }
    if (-not (Test-Path -LiteralPath $executable)) {
        stop-stage "$stage missing output $executable"
    }
    $code_exit = 1
    push-location $path_work
    try {
        & $executable
        $code_exit = $LASTEXITCODE
    }
    finally {
        pop-location
    }
    if ($code_exit -ne 0) {
        stop-stage "$stage run" $code_exit
    }
    Write-Host "PASSED: $stage"
}

if ($env:GENCPP_SOURCE_ROOT) {
    $path_root = (Resolve-Path -LiteralPath $env:GENCPP_SOURCE_ROOT).Path
    $probe = Join-Path $path_root "base\base.cpp"
    if (-not (Test-Path -LiteralPath $probe)) {
        stop-stage "GENCPP_SOURCE_ROOT is not a gencpp tree: $path_root"
    }
}
else {
    $path_root = Get-ScriptRepoRoot
}
Write-Host "SOURCE_ROOT: $path_root"
if ($null -eq $is_windows) {
    $is_windows = [System.Environment]::OSVersion.Platform -eq 'Win32NT'
}
if ($null -eq $is_linux) {
    $is_linux = [System.Environment]::OSVersion.Platform -eq 'Unix'
}

Push-Location $path_root

#region Arguments
       $vendor               = $null
       $release              = $null
[bool] $verbose              = $false
[bool] $base                 = $false
[bool] $segmented            = $false
[bool] $singleheader         = $false
[bool] $c_lib                = $false
[bool] $c_lib_static         = $false
[bool] $c_lib_dyn            = $false
[bool] $unreal               = $false
[bool] $test                 = $false
[bool] $parser_bounds        = $false
[bool] $lexer_failures       = $false
[bool] $parse_body_messages  = $false
[bool] $dogfood_parse        = $false

[array] $vendors = @( "clang", "msvc" )

# This is a really lazy way of parsing the args, could use actual params down the line...

if ( $args ) { $args | ForEach-Object {
	switch ($_){
		{ $_ -in $vendors }   { $vendor               = $_; break }
		"verbose"			  { $verbose              = $true }
		"release"             { $release              = $true }
		"debug"               { $release              = $false }
		"base"                { $base                 = $true }
		"segmented"           { $segmented            = $true }
		"singleheader"        { $singleheader         = $true }
		"c_lib"               { $c_lib                = $true }
		"c_lib_static"        { $c_lib_static         = $true }
		"c_lib_dyn"           { $c_lib_dyn            = $true }
		"unreal"              { $unreal               = $true }
		"test"                { $test                 = $true }
		"parser_bounds"       { $parser_bounds        = $true }
		"lexer_failures"      { $lexer_failures       = $true }
		"parse_body_messages" { $parse_body_messages  = $true }
		"dogfood_parse"       { $dogfood_parse        = $true }
	}
}}
#endregion Arguments

#region Configuration
if ($is_windows) {
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
	$debug = $true
}
elseif ( $release -eq $false ) {
	$debug = $true
}
else {
	$optimize = $true
}

$cannot_build =                     $base                -eq $false
$cannot_build = $cannot_build -and  $segmented           -eq $false
$cannot_build = $cannot_build -and  $singleheader        -eq $false
$cannot_build = $cannot_build -and  $c_lib               -eq $false
$cannot_build = $cannot_build -and  $c_lib_static        -eq $false
$cannot_build = $cannot_build -and  $c_lib_dyn           -eq $false
$cannot_build = $cannot_build -and  $unreal              -eq $false
$cannot_build = $cannot_build -and  $test                -eq $false
$cannot_build = $cannot_build -and  $parser_bounds       -eq $false
$cannot_build = $cannot_build -and  $lexer_failures      -eq $false
$cannot_build = $cannot_build -and  $parse_body_messages -eq $false
$cannot_build = $cannot_build -and  $dogfood_parse       -eq $false
if ( $cannot_build ) {
	Stop-stage "No build target specified"
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
	invoke-stageTool -stage "base" -compiled $result -executable $executable -path_work $path_base
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
	invoke-stageTool -stage "segmented" -compiled $result -executable $executable -path_work $path_segmented
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
	invoke-stageTool -stage "singleheader" -compiled $result -executable $executable -path_work $path_singleheader
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
	invoke-stageTool -stage "c_lib" -compiled $result -executable $executable -path_work $path_c_library
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
	$compiler_args += ($flag_define + 'GEN_STATIC_LINK')

	$linker_args = @()
	$result = build-simple $path_build $includes $compiler_args $linker_args $unit $path_lib
	if (-not $result) { Stop-stage "c_lib_static compile/link" }
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
	$compiler_args += ( $flag_define + 'GEN_DEFINE_LIBRARY_CODE_CONSTANTS' )
 
	$linker_args = @()
	$result = build-simple $path_build $includes $compiler_args $linker_args $unit $path_dll
	if (-not $result) { Stop-stage "c_lib_dyn compile/link" }
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
	invoke-stageTool -stage "unreal" -compiled $result -executable $executable -path_work $path_unreal

	. $refactor_unreal
}

# C Library testing
if ($test) {
    $consumer_headers = @(
        (Join-Path $path_c_library 'gen\gen_singleheader.h'),
        (Join-Path $path_singleheader 'gen\gen.hpp')
    )
    $header_hashes_before = @{}
    foreach ($header in $consumer_headers) {
        if (-not (Test-Path -LiteralPath $header -PathType Leaf)) {
            Stop-stage "test missing generated header $header"
        }
        $header_hashes_before[$header] = (Get-FileHash -LiteralPath $header -Algorithm SHA256).Hash
        Write-Host "HEADER_SHA256_GENERATED: $($header_hashes_before[$header]) $header"
    }
}

if ( $test -and $true )
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
	invoke-stageTool -stage "test-c11" -compiled $result -executable $executable -path_work $path_test_c
}

if ( $test -and $false )
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
	$unit       = join-path $path_test_c "test_cuik.c"
	$executable = join-path $path_build  "test_cuik.exe"

	$compiler_args = @()
	$compiler_args += ( $flag_define + 'GEN_TIME' )
	$compiler_args += $flag_all_c
	$compiler_args += $flag_updated_cpp_macro
	$compiler_args += $flag_c11

	$linker_args   = @(
		$flag_link_win_subsystem_console
	)

	$result = build-simple $path_build $includes $compiler_args $linker_args $unit $executable
}

if ($test)
{
	$path_test_cpp = join-path $path_test     cpp_library
	$path_build    = join-path $path_test_cpp build
	$path_gen      = join-path $path_test_cpp gen
	if ( -not(Test-Path($path_build) )) {
		new-item -ItemType Directory -Path $path_build
	}
	if ( -not(Test-Path($path_gen) )) {
		new-item -ItemType Directory -Path $path_gen
	}

	$path_singleheader_include = join-path $path_singleheader gen
	$includes    = @( $path_singleheader_include )
	$unit       = join-path $path_test_cpp "test.cpp"
	$executable = join-path $path_build    "test.exe"

	$compiler_args = @()
	$compiler_args += ( $flag_define + 'GEN_TIME' )
	$compiler_args += $flag_all_cpp
	$compiler_args += $flag_cpp17
	$compiler_args += ($flag_define + "GEN_BUILD_DEBUG=1")

	$linker_args   = @(
		$flag_link_win_subsystem_console
	)

	$result = build-simple $path_build $includes $compiler_args $linker_args $unit $executable
	invoke-stageTool -stage "test-cpp" -compiled $result -executable $executable -path_work $path_test_cpp
}
if ($test) {
    foreach ($header in $consumer_headers) {
        $hash = (Get-FileHash -LiteralPath $header -Algorithm SHA256).Hash
        write-host "HEADER_SHA256_CONSUMED: $hash $header"
        if ($hash -ne $header_hashes_before[$header]) {
            Stop-stage "test header changed after generation $header"
        }
    }
}

if ($parser_bounds)
{
	$path_probe = join-path $path_test parser_bounds
	$path_build = join-path $path_probe build
	if ( -not(Test-Path($path_build) )) {
		new-item -ItemType Directory -Path $path_build | Out-Null
	}

	$includes   = @( $path_base )
	$unit       = join-path $path_probe "test.cpp"
	$executable = join-path $path_build "parser_bounds.exe"

	$compiler_args = @()
	$compiler_args += ( $flag_define + 'GEN_TIME' )
	$compiler_args += $flag_cpp17

	$linker_args = @(
		$flag_link_win_subsystem_console
	)

	$result = build-simple $path_build $includes $compiler_args $linker_args $unit $executable
	invoke-stageTool -stage "parser_bounds" -compiled $result -executable $executable -path_work $path_probe
}

if ($lexer_failures)
{
	$path_probe = join-path $path_test lexer_failures
	$path_build = join-path $path_probe build
	if ( -not(Test-Path($path_build) )) {
		new-item -ItemType Directory -Path $path_build | Out-Null
	}

	$includes   = @( $path_base )
	$unit       = join-path $path_probe "test.cpp"
	$executable = join-path $path_build "lexer_failures.exe"

	$compiler_args = @()
	$compiler_args += ( $flag_define + 'GEN_TIME' )
	$compiler_args += $flag_cpp17

	$linker_args = @(
		$flag_link_win_subsystem_console
	)

	$result = build-simple $path_build $includes $compiler_args $linker_args $unit $executable
	invoke-stageTool -stage "lexer_failures" -compiled $result -executable $executable -path_work $path_probe
}

if ($parse_body_messages)
{
	$path_probe = join-path $path_test parse_body_messages
	$path_build = join-path $path_probe build
	if ( -not(Test-Path($path_build) )) {
		new-item -ItemType Directory -Path $path_build | Out-Null
	}

	$includes   = @( $path_base )
	$unit       = join-path $path_probe "test.cpp"
	$executable = join-path $path_build "parse_body_messages.exe"

	$compiler_args = @()
	$compiler_args += ( $flag_define + 'GEN_TIME' )
	$compiler_args += $flag_cpp17

	$linker_args = @(
		$flag_link_win_subsystem_console
	)

	$result = build-simple $path_build $includes $compiler_args $linker_args $unit $executable
	invoke-stageTool -stage "parse_body_messages" -compiled $result -executable $executable -path_work $path_probe
}

if ($dogfood_parse)
{
	$path_probe = join-path $path_test dogfood_parse
	$path_build = join-path $path_probe build
	if ( -not(Test-Path($path_build) )) {
		new-item -ItemType Directory -Path $path_build | Out-Null
	}

	$includes   = @( $path_base )
	$unit       = join-path $path_probe "test.cpp"
	$executable = join-path $path_build "dogfood_parse.exe"

	$compiler_args = @()
	$compiler_args += ( $flag_define + 'GEN_TIME' )
	$compiler_args += $flag_cpp17

	$linker_args = @(
		$flag_link_win_subsystem_console
	)

	$result = build-simple $path_build $includes $compiler_args $linker_args $unit $executable
	invoke-stageTool -stage "dogfood_parse" -compiled $result -executable $executable -path_work $path_probe
}
#endregion Building

pop-location # $path_root
write-host "PASSED: build.ci.ps1"
exit 0
