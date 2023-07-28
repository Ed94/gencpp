$path_root               = git rev-parse --show-toplevel
$path_project            = Join-Path $path_root project
$path_project_build      = Join-Path $path_project build
$path_project_gen		 = Join-Path $path_project gen
$path_singleheader       = Join-Path $path_root singleheader
$path_singleheader_build = Join-Path $path_singleheader build
$path_singleheader_gen	 = Join-Path $path_singleheader gen
$path_test               = Join-Path $path_root test
$path_test_build         = Join-Path $path_test build
$path_gen                = Join-Path $path_test gen
$path_gen_build          = Join-Path $path_gen  build
$path_x64				 = Join-Path $path_root x64

if ( Test-Path $path_project_build)
{
	Remove-Item $path_project_build -Recurse
}

if ( Test-Path $path_project_gen )
{
	Remove-Item $path_project_gen -Recurse
}

if ( Test-Path $path_singleheader_build)
{
	Remove-Item $path_singleheader_build -Recurse
}

if ( Test-Path $path_singleheader_gen )
{
	Remove-Item $path_singleheader_gen -Recurse
}

if ( Test-Path $path_test_build )
{
	Remove-Item $path_test_build -Recurse
}

if ( Test-Path $path_gen_build )
{
	Remove-Item $path_gen_build -Recurse
}

if ( Test-Path $path_x64)
{
	Remove-Item $path_x64 -Recurse
}

# [string[]] $include = 'gen.hpp', 'gen.cpp', 'gen_dep.hpp', 'gen_dep.cpp'
# [string[]] $exclude =

# $files = Get-ChildItem -Recurse -Path $path_project_gen -Include $include -Exclude $exclude

# if ( $files )
# {
# 	Remove-Item $files
# }

# $files = Get-ChildItem -Recurse -Path $path_singleheader_gen -Include $include -Exclude $exclude

# if ( $files )
# {
# 	Remove-Item $files
# }

$include = '*.h', '*.hpp', '*.cpp'
$exclude =

$files = Get-ChildItem -Recurse -Path $path_gen -Include $include -Exclude $exclude

if ( $files )
{
	Remove-Item $files
}
