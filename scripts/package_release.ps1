cls

$build = Join-Path $PSScriptRoot 'build.ci.ps1'

if ( $IsWindows ) {
	& $build release msvc bootstrap singleheader
}
else {
	& $build release clang bootstrap singleheader
}

$path_root             = git rev-parse --show-toplevel
$path_docs			   = Join-Path $path_root docs
$path_project          = Join-Path $path_root project
$path_project_gen      = Join-Path $path_project gen
$path_singleheader	   = Join-Path $path_root singleheader
$path_singleheader_gen = Join-Path $path_singleheader gen
$path_release          = Join-Path $path_root release
$path_release_content  = Join-Path $path_release content

if ( -not(Test-Path $path_release) ) {
	New-Item -ItemType Directory -Path $path_release
}

if ( -not(Test-Path $path_release_content) ) {
	New-Item -ItemType Directory -Path $path_release_content
}

$license            = Join-Path $path_root LICENSE
$readme_root        = Join-Path $path_root Readme.md
$readme_docs        = Join-Path $path_docs Readme.md
$readme_ast_design  = Join-Path $path_docs AST_Design.md
$readme_ast_types   = Join-Path $path_docs AST_Types.md
$readme_parsing     = Join-Path $path_docs Parsing.md
$readme_parser_algo = Join-Path $path_docs Parser_Algo.md

Copy-Item $license		      -Destination (Join-Path $path_release_content "LICENSE")
Copy-Item $readme_root        -Destination (Join-Path $path_release_content "Readme.md")
Copy-Item $readme_docs        -Destination (Join-Path $path_release_content "Readme_Docs.md")
Copy-Item $readme_ast_design  -Destination (Join-Path $path_release_content "AST_Design.md")
Copy-Item $readme_ast_types   -Destination (Join-Path $path_release_content "AST_Types.md")
Copy-Item $readme_parsing     -Destination (Join-Path $path_release_content "Parsing.md")
Copy-Item $readme_parser_algo -Destination (Join-Path $path_release_content "Parser_Algo.md")

# Singleheader
Copy-Item        -Path $path_singleheader_gen\gen.hpp -Destination $path_release_content\gen.hpp
Compress-Archive -Path $path_release_content\*        -DestinationPath $path_release\gencpp_singleheader.zip -Force
Remove-Item      -Path $path_release_content\gen.hpp

# Segmented
Copy-Item        -Path $path_project_gen\*     -Destination $path_release_content
Compress-Archive -Path $path_release_content\* -DestinationPath $path_release\gencpp_segmented.zip -Force

Remove-Item -Path $path_release_content -Recurse
