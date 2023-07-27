cls

if ( -not( Test-Path $path_build ) ) {
	New-Item -ItemType Directory -Path $path_build | Out-Null
}
if ( -not( Test-Path $path_build_interm ) ) {
	New-Item -ItemType Directory -Path $path_build_interm | Out-Null
}

$path_root       = git rev-parse --show-toplevel
$path_build      = Join-Path $path_root build
$path_gen        = Join-Path $path_test gen
$path_gen_build  = Join-Path $path_gen  build
$path_scripts    = Join-Path $path_root scripts
$path_test       = Join-Path $path_root test
$path_test_build = Join-Path $path_test build
