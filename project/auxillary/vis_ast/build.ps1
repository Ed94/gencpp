Clear-Host

$path_root     = git rev-parse --show-toplevel
$path_scripts  = Join-Path $path_root 'scripts'

$target_arch        = Join-Path $path_scripts 'helpers/target_arch.psm1'
$devshell           = Join-Path $path_scripts 'helpers/devshell.ps1'
$format_cpp	        = Join-Path $path_scripts 'helpers/format_cpp.psm1'
$incremental_checks = Join-Path $path_scripts 'helpers/incremental_checks.ps1'
$vendor_toolchain   = Join-Path $path_scripts 'helpers/vendor_toolchain.ps1'

$path_project  = Join-Path $path_root     'project'
$path_aux      = Join-Path $path_project  'auxillary'
$path_vis_root = Join-Path $path_aux      'vis_ast'
$path_binaries = Join-Path $path_vis_root 'binaries'
$path_build    = Join-Path $path_vis_root 'build'
$path_code     = Join-Path $path_vis_root 'code'
$path_win32    = Join-Path $path_code     'win32'

Import-Module $target_arch
Import-Module $format_cpp

#region Arguments
$vendor           = $null
$optimize         = $null
$debug 	          = $null
$analysis	      = $false
$dev              = $false
$verbose          = $null
$platform         = $null
$module_specified = $false

[array] $vendors = @( "clang", "msvc" )

# This is a really lazy way of parsing the args, could use actual params down the line...

if ( $args ) { $args | ForEach-Object {
switch ($_){
 { $_ -in $vendors }   { $vendor    = $_; break }
 "optimize"            { $optimize  = $true }
 "debug"               { $debug     = $true }
 "analysis"            { $analysis  = $true }
 "dev"                 { $dev       = $true }
 "verbose"             { $verbose   = $true }
 "platform"            { $platform  = $true; $module_specified = $true }
}
}}
#endregion Argument

if ( -not $module_specified )
{
	$platform = $true
}

# Load up toolchain configuraion
. $vendor_toolchain
. $incremental_checks

write-host "Building Vis AST with $vendor"

if ( (Test-Path $path_build) -eq $false ) {
	New-Item $path_build -ItemType Directory
}

if ( (Test-Path $path_binaries) -eq $false ) {
	New-Item $path_binaries -ItemType Directory
}

$includes = @(
	$paht_code
)

# Microsoft
$lib_gdi32  = 'Gdi32.lib'
$lib_xinput = 'Xinput.lib'
$lib_user32 = 'User32.lib'
$lib_winmm  = 'Winmm.lib'

$stack_size = 1024 * 1024 * 4

$compiler_args = @(
	($flag_define + 'UNICODE'),
	($flag_define + '_UNICODE')
	( $flag_define + 'INTELLISENSE_DIRECTIVES=0'),
	# ($flag_set_stack_size + $stack_size)
	$flag_wall
	$flag_warnings_as_errors
	$flag_optimize_intrinsics
)

if ( $dev ) {
	$compiler_args += ( $flag_define + 'Build_Development=1' )
}
else {
	$compiler_args += ( $flag_define + 'Build_Development=0' )
}

$linker_args = @(
	$flag_link_win_subsystem_windows,
	$flag_link_optiiize_references
)

$unit       = join-path $path_code     'vis_ast_windows.cpp'
$executable = join-path $path_binaries 'vis_ast.exe'

$build_result = build-simple $path_build $includes $compiler_args $linker_args $unit $executable
