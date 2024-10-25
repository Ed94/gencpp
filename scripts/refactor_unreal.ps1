[string] $format = $false

foreach ( $arg in $args )
{
	if ( $arg -eq "format" )
	{
		$format = $true
	}
}

[string[]] $include = 'gen.*.hpp', 'gen.*.cpp', 'gen.hpp', 'gen.cpp'
[string[]] $exclude

$path_root               = git rev-parse --show-toplevel
$path_project            = Join-Path $path_root        project
$path_scripts            = Join-Path $path_root        scripts
$path_singlheader        = Join-Path $path_root        singleheader
$path_singleheader_comp  = Join-Path $path_singlheader components
$path_unreal             = Join-Path $path_root        unreal_engine
$path_unreal_gen         = Join-Path $path_unreal      gen

$file_spec = Join-Path $path_scripts unreal.refactor

# Gather the files to be formatted.
$targetFiles = @()
$targetFiles += Get-ChildItem -Recurse -Path $path_unreal_gen        -Include $include -Exclude $exclude | Select-Object -ExpandProperty FullName
# $targetFiles += Get-ChildItem -Recurse -Path $path_project           -Include $include -Exclude $exclude | Select-Object -ExpandProperty FullName
# $targetFiles += Get-ChildItem -Recurse -Path $path_singleheader_comp -Include $include -Exclude $exclude | Select-Object -ExpandProperty FullName

# Format the files.
$formatParams = @(
    '-i'                          # In-place
    '-style=file:./.clang-format' # Search for a .clang-format file in the parent directory of the source file.
    '-verbose'
)

write-host "Beginning refactor...`n"

$refactorParams = @(
    # "-debug",
    "-num=$($targetFiles.Count)"
    "-src=$($targetFiles)",
    "-spec=$($file_spec)"
)

& refactor $refactorParams

Write-Host "`nRefactoring complete`n`n"

if ( $format -eq $true ) {
	Write-Host "Beginning format...`n"

	& clang-format $formatParams $targetFiles

	Write-Host "`nFormatting complete"
}
