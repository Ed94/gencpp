$misc = Join-Path $PSScriptRoot 'helpers/misc.psm1'
Import-Module $misc

$build = Join-Path $PSScriptRoot 'build.ci.ps1'

if ( $IsWindows ) {
	& $build release msvc debug base segmented singleheader unreal c_lib c_lib_static c_lib_dyn
}

$path_root             = Get-ScriptRepoRoot
$path_docs			   = Join-Path $path_root          docs
$path_base             = Join-Path $path_root          base
$path_c_library        = Join-Path $path_root          gen_c_library
$path_c_library_build  = Join-Path $path_c_library     build
$path_c_library_gen    = Join-Path $path_c_library     gen
$path_segmented        = Join-Path $path_root          gen_segmented
$path_segmented_gen    = Join-Path $path_segmented     gen
$path_singleheader	   = Join-Path $path_root          gen_singleheader
$path_singleheader_gen = Join-Path $path_singleheader  gen
$path_unreal           = Join-Path $path_root          gen_unreal_engine
$path_unreal_gen       = Join-Path $path_unreal        gen
$path_release          = Join-Path $path_root          release
$path_release_content  = Join-Path $path_release       content

if ( -not(Test-Path $path_release) ) {
	New-Item -ItemType Directory -Path $path_release
}

function prep-ReleaseContent()
{
	New-Item -ItemType Directory -Path $path_release_content

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
}

# Singleheader
prep-ReleaseContent
Copy-Item        -Verbose -Path $path_singleheader\Readme.md   -Destination $path_release_content
Copy-Item        -Verbose -Path $path_singleheader_gen\gen.hpp -Destination $path_release_content
Compress-Archive -Path $path_release_content\*        -DestinationPath $path_release\gencpp_singleheader.zip -Force
Remove-Item -Path $path_release_content -Recurse

# Segmented
prep-ReleaseContent
Copy-Item        -Verbose -Path $path_segmented\Readme.md   -Destination $path_release_content
Copy-Item        -Verbose -Path $path_segmented_gen\*       -Destination $path_release_content
Compress-Archive -Path $path_release_content\*     -DestinationPath $path_release\gencpp_segmented.zip -Force
Remove-Item -Path $path_release_content -Recurse

# Unreal
prep-ReleaseContent
Copy-Item        -Verbose -Path $path_unreal\Readme.md  -Destination $path_release_content
Copy-Item        -Verbose -Path $path_unreal_gen\*      -Destination $path_release_content
Compress-Archive -Path $path_release_content\* -DestinationPath $path_release\gencpp_unreal.zip -Force
Remove-Item -Path $path_release_content -Recurse

# C Library Singleheader
prep-ReleaseContent
Copy-Item        -Verbose -Path $path_c_library\Readme.md              -Destination $path_release_content
Copy-Item        -Verbose -Path $path_c_library_gen\gen_singleheader.h -Destination $path_release_content\gen.h
Compress-Archive -Path $path_release_content\*                         -DestinationPath $path_release\gencpp_c11_singleheader.zip -Force
Remove-Item -Path $path_release_content -Recurse

# C Library Segmented
prep-ReleaseContent
Copy-Item        -Verbose -Path $path_c_library\Readme.md     -Destination $path_release_content
Copy-Item        -Verbose -Path $path_c_library_gen\gen.dep.c -Destination $path_release_content
Copy-Item        -Verbose -Path $path_c_library_gen\gen.dep.h -Destination $path_release_content
Copy-Item        -Verbose -Path $path_c_library_gen\gen.c     -Destination $path_release_content
Copy-Item        -Verbose -Path $path_c_library_gen\gen.h     -Destination $path_release_content
Compress-Archive -Path $path_release_content\*       -DestinationPath $path_release\gencpp_c11_segmented.zip -Force
Remove-Item -Path $path_release_content -Recurse

# C Library Segmented
prep-ReleaseContent
Copy-Item        -Verbose -Path $path_c_library\Readme.md     -Destination $path_release_content
Copy-Item        -Verbose -Path $path_c_library_gen\gen.dep.c -Destination $path_release_content
Copy-Item        -Verbose -Path $path_c_library_gen\gen.dep.h -Destination $path_release_content
Copy-Item        -Verbose -Path $path_c_library_gen\gen.c     -Destination $path_release_content
Copy-Item        -Verbose -Path $path_c_library_gen\gen.h     -Destination $path_release_content
Compress-Archive -Path $path_release_content\*                -DestinationPath $path_release\gencpp_c11_segmented.zip -Force
Remove-Item -Path $path_release_content -Recurse

# C Lib Static & Dyanmic Libs
Copy-Item -Verbose -Path $path_c_library_build\gencpp_c11.lib -Destination $path_release
Copy-Item -Verbose -Path $path_c_library_build\gencpp_c11.dll -Destination $path_release

# Base

prep-ReleaseContent
Copy-Item -Verbose -Path $path_base\gen.hpp               -Destination $path_release_content
Copy-Item -Verbose -Path $path_base\gen.cpp               -Destination $path_release_content
Copy-Item -Verbose -Path $path_base\gen.dep.hpp           -Destination $path_release_content
Copy-Item -Verbose -Path $path_base\gen.dep.cpp           -Destination $path_release_content
Copy-Item -Verbose -Path $path_base\auxiliary\builder.hpp -Destination $path_release_content\auxiliary
Copy-Item -Verbose -Path $path_base\auxiliary\builder.cpp -Destination $path_release_content\auxiliary
Copy-Item -Verbose -Path $path_base\auxiliary\scanner.hpp -Destination $path_release_content\auxiliary
Copy-Item -Verbose -Path $path_base\auxiliary\scanner.cpp -Destination $path_release_content\auxiliary

New-Item -ItemType Directory -Force -Path "$path_release_content\components"
New-Item -ItemType Directory -Force -Path "$path_release_content\components\gen"
New-Item -ItemType Directory -Force -Path "$path_release_content\dependencies"
New-Item -ItemType Directory -Force -Path "$path_release_content\enums"
New-Item -ItemType Directory -Force -Path "$path_release_content\helpers"

Get-ChildItem -Verbose -Path "$path_base\components\*"     -Include *.cpp,*.hpp | Copy-Item -Verbose -Destination "$path_release_content\components"
Get-ChildItem -Verbose -Path "$path_base\components\gen\*" -Include *.cpp,*.hpp | Copy-Item -Verbose -Destination "$path_release_content\components\gen"
Get-ChildItem -Verbose -Path "$path_base\dependencies\*"   -Include *.cpp,*.hpp | Copy-Item -Verbose -Destination "$path_release_content\dependencies"
Get-ChildItem -Verbose -Path "$path_base\enums\*"          -Include *.csv       | Copy-Item -Verbose -Destination "$path_release_content\enums"
Get-ChildItem -Verbose -Path "$path_base\helpers\*"        -Include *.cpp,*.hpp | Copy-Item -Verbose -Destination "$path_release_content\helpers"

Compress-Archive -Path $path_release_content\** -DestinationPath $path_release\gencpp_base.zip -Force
Remove-Item -Path $path_release_content -Recurse
