[string] $format = $false

foreach ( $arg in $args )
{
	if ( $arg -eq "format" )
	{
		$format = $true
	}
}

[string[]] $include = 'gen.h'
[string[]] $exclude

$path_root               = git rev-parse --show-toplevel
$path_project            = Join-Path $path_root        project
$path_scripts            = Join-Path $path_root        scripts
$path_helpers            = Join-Path $path_scripts     helpers
$path_c_library          = Join-Path $path_root        gen_c_library
$path_c_library_gen      = Join-Path $path_c_library   gen

$file_spec = Join-Path $path_c_library c_library.refactor

# Gather the files to be formatted.
$targetFiles = @()
$targetFiles += Get-ChildItem -Recurse -Path $path_c_library_gen        -Include $include -Exclude $exclude | Select-Object -ExpandProperty FullName
# $targetFiles += Get-ChildItem -Recurse -Path $path_project           -Include $include -Exclude $exclude | Select-Object -ExpandProperty FullName
# $targetFiles += Get-ChildItem -Recurse -Path $path_singleheader_comp -Include $include -Exclude $exclude | Select-Object -ExpandProperty FullName

# Format the files.
$formatParams = @(
    '-i'                          # In-place
    '-style=file:./.clang-format' # Search for a .clang-format file in the parent directory of the source file.
    '-verbose'
)

write-host "Beginning refactor...`n"

Write-Host $targetFiles

$refactorParams = @(
    "-debug",
    "-num=$($targetFiles.Count)"
    "-src=$($targetFiles)",
    "-spec=$($file_spec)"
)

$refactor = join-path $path_helpers refactor.exe
write-host "& $refactor $refactorParams"
& $refactor $refactorParams

Write-Host "`nRefactoring complete`n`n"

if ( $format -eq $true ) {
	Write-Host "Beginning format...`n"

	& clang-format $formatParams $targetFiles

	Write-Host "`nFormatting complete"
}
