$path_root       = git rev-parse --show-toplevel
$path_build      = Join-Path $path_root build
$path_test       = Join-Path $path_root test
$path_test_build = Join-Path $path_test build

if ( Test-Path $path_build ) 
{
	Remove-Item $path_build -Recurse 
}

if ( Test-Path $path_test_build )
{
	Remove-Item $path_test_build -Recurse
}

# [string[]] $include = '*.h', '*.hpp', '*.cpp'
# [string[]] $exclude =

# $files = Get-ChildItem -Recurse -Path $path_test -Include $include -Exclude $exclude

# if ( $files )
# {
# 	Remove-Item $files
# }
