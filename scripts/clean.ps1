$misc = Join-Path $PSScriptRoot 'helpers/misc.psm1'
Import-Module $misc

$path_root               = Get-ScriptRepoRoot
$path_base               = Join-Path $path_root         base
$path_base_build         = Join-Path $path_base         build
$path_c_library          = Join-Path $path_root         gen_c_library
$path_c_library_build    = Join-Path $path_c_library    build
$path_c_library_gen      = Join-Path $path_c_library    gen
$path_segmented          = Join-Path $path_root         gen_segmented
$path_segmented_build    = Join-Path $path_segmented    build
$path_segmented_gen      = Join-Path $path_segmented    gen
$path_singleheader       = Join-Path $path_root         gen_singleheader
$path_singleheader_build = Join-Path $path_singleheader build
$path_singleheader_gen	 = Join-Path $path_singleheader gen
$path_unreal             = Join-Path $path_root         gen_unreal_engine
$path_unreal_build       = Join-Path $path_unreal       build
$path_unreal_gen         = Join-Path $path_unreal       gen
$path_test               = Join-Path $path_root         test
$path_test_build         = Join-Path $path_test         build
$path_test_gen           = Join-Path $path_test         gen
$path_x64				 = Join-Path $path_root         x64
$path_release			 = Join-Path $path_root         release

if ( Test-Path $path_base_build) {
	Remove-Item $path_base_build -Recurse -Verbose
}
if ( Test-Path $path_segmented_build) {
	Remove-Item $path_segmented_build -Recurse -Verbose
}
if ( Test-Path $path_c_library_build ) {
	Remove-Item $path_c_library_build -Recurse -Verbose
}
if ( Test-Path $path_c_library_gen ) {
	Remove-Item $path_c_library_gen -Recurse -Verbose
}
if ( Test-Path $path_segmented_build) {
	Remove-Item $path_segmented_build -Recurse -Verbose
}
if ( Test-Path $path_segmented_gen ) {
	Remove-Item $path_segmented_gen -Recurse -Verbose
}
if ( Test-Path $path_singleheader_build) {
	Remove-Item $path_singleheader_build -Recurse -Verbose
}
if ( Test-Path $path_singleheader_gen ) {
	Remove-Item $path_singleheader_gen -Recurse -Verbose
}
if ( Test-Path $path_unreal_build ) {
	Remove-Item $path_unreal_build -Recurse -Verbose
}
if ( Test-Path $path_unreal_gen ) {
	Remove-Item $path_unreal_gen -Recurse -Verbose
}
if ( Test-Path $path_test_build ) {
	Remove-Item $path_test_build -Recurse -Verbose
}
if ( Test-Path $path_test_gen ) {
	Remove-Item $path_test_gen -Recurse -Verbose
}
if ( Test-Path $path_x64) {
	Remove-Item $path_x64 -Recurse -Verbose
}
if ( Test-Path $path_release ) {
	Remove-Item $path_release -Recurse -Verbose
}
