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

if ( test-path $path_build ) {
	remove-item $path_build -Recurse
}

if ( test-path $path_binaries ) {
	remove-item $path_binaries -recurse
}
