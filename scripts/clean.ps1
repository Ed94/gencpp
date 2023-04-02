$path_root       = git rev-parse --show-toplevel
$path_build      = Join-Path $path_root build
$path_test       = Join-Path $path_root test
$path_gen        = Join-Path $path_test gen
$path_test_build = Join-Path $path_test build
$path_gen_build  = Join-Path $path_gen  build

if ( Test-Path $path_build ) 
{
	Remove-Item $path_build -Recurse 
}

if ( Test-Path $path_test_build )
{
	Remove-Item $path_test_build -Recurse
}

if ( Test-Path $path_gen_build )
{
	Remove-Item $path_gen_build -Recurse
}

[string[]] $include = '*.h', '*.hpp', '*.cpp'
[string[]] $exclude =

$files = Get-ChildItem -Recurse -Path $path_gen -Include $include -Exclude $exclude

if ( $files )
{
	Remove-Item $files
}
