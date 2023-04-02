[string[]] $include = 'gen.cpp' #'*.c', '*.cc', '*.cpp'
# [string[]] $exclude =

$path_root = git rev-parse --show-toplevel
$path_proj = Join-Path $path_root project

$files = Get-ChildItem -Recurse -Path $path_proj -Include $include -Exclude $exclude

$sources = $files | Select-Object -ExpandProperty FullName | Resolve-Path -Relative
$sources = $sources.Replace( '\', '/' )

return $sources
