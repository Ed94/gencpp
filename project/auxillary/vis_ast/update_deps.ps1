$path_root     = git rev-parse --show-toplevel
$path_project  = Join-Path $path_root     'project'
$path_aux      = Join-Path $path_project  'auxillary'
$path_vis_root = Join-Path $path_aux      'vis_ast'
$path_binaries = Join-Path $path_vis_root 'binaries'
$path_build    = Join-Path $path_vis_root 'build'
$path_code     = Join-Path $path_vis_root 'code'
$path_win32    = Join-Path $path_code     'win32'

$path_deps = Join-Path $path_vis_root 'dependencies'
$path_temp = Join-Path $path_deps     'temp'

# Clear out the current content first
if (Test-Path $path_deps) {
	Remove-Item $path_deps -Recurse -Force
	New-Item -ItemType Directory -Path $path_deps
}
New-Item -ItemType Directory -Path $path_temp

if ( -not (Test-Path $path_binaries) ) {
	New-Item -ItemType Directory -Path $path_binaries
}

$url_raylib_zip  = 'https://github.com/raysan5/raylib/archive/refs/heads/master.zip'
$path_raylib_zip = join-path $path_temp 'raylib.zip'

invoke-webrequest -uri $url_raylib_zip -outfile $path_raylib_zip

