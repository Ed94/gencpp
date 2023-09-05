# This build script was written to build on windows, however I did setup some generalization to allow for cross platform building.
# It will most likely need a partial rewrite to segment the build process into separate script invocations based on the OS.
# That or just rewrite it in an sh script and call it a day.

Import-Module ./helpers/target_arch.psm1
$devshell  = Join-Path $PSScriptRoot 'helpers/devshell.ps1'
$path_root = git rev-parse --show-toplevel

Push-Location $path_root

#region Arguments
       $vendor       = $null
       $release      = $null
[bool] $bootstrap    = $false
[bool] $singleheader = $false
[bool] $test         = $false

[array] $vendors = @( "clang", "msvc" )

# This is a really lazy way of parsing the args, could use actual params down the line...

if ( $args ) { $args | ForEach-Object {
	switch ($_){
		{ $_ -in $vendors }   { $vendor      = $_; break }
		"release"             { $release      = $true }
		"debug"               { $release      = $false }
		"bootstrap"           { $bootstrap    = $true }
		"singleheader"        { $singleheader = $true }
		"test"                { $test         = $true }
	}
}}
#endregion Arguments

#region Configuration
if ($IsWindows) {
	# This library was really designed to only run on 64-bit systems.
	# (Its a development tool after all)
    & $devshell -arch amd64
}

if ( $vendor -eq $null ) {
	write-host "No vendor specified, assuming clang available"
	$compiler = "clang"
}

if ( $release -eq $null ) {
	write-host "No build type specified, assuming debug"
	$release = $false
}

if ( $bootstrap -eq $false -and $singleheader -eq $false -and $test -eq $false ) {
	throw "No build target specified. One must be specified, this script will not assume one"
}

write-host "Building gencpp with $vendor"
write-host "Build Type: $(if ($release) {"Release"} else {"Debug"} )"

function run-compiler
{
	param( $compiler, $unit, $compiler_args )

	write-host "`Compiling $unit"
	write-host "Compiler config:"
	$compiler_args | ForEach-Object {
		write-host $_ -ForegroundColor Cyan
	}

	$time_taken = Measure-Command {
		& $compiler $compiler_args 2>&1 | ForEach-Object {
			$color = 'White'
			switch ($_){
				{ $_ -match "error"   } { $color = 'Red'   ; break }
				{ $_ -match "warning" } { $color = 'Yellow'; break }
			}
			write-host `t $_ -ForegroundColor $color
		}
	}

	if ( Test-Path($unit) ) {
		write-host "$unit compile finished in $($time_taken.TotalMilliseconds) ms"
	}
	else {
		write-host "Compile failed for $unit" -ForegroundColor Red
	}
}

function run-linker
{
	param( $linker, $binary, $linker_args )

	write-host "`Linking $binary"
	write-host "Linker config:"
	$linker_args | ForEach-Object {
		write-host $_ -ForegroundColor Cyan
	}

	$time_taken = Measure-Command {
		& $linker $linker_args 2>&1 | ForEach-Object {
			$color = 'White'
			switch ($_){
				{ $_ -match "error"   } { $color = 'Red'   ; break }
				{ $_ -match "warning" } { $color = 'Yellow'; break }
			}
			write-host `t $_ -ForegroundColor $color
		}
	}

	if ( Test-Path($binary) ) {
		write-host "$binary linking finished in $($time_taken.TotalMilliseconds) ms"
	}
	else {
		write-host "Linking failed for $binary" -ForegroundColor Red
	}
}

if ( $vendor -match "clang" )
{
	# https://clang.llvm.org/docs/ClangCommandLineReference.html
	$flag_compile                    = '-c'
	$flag_color_diagnostics          = '-fcolor-diagnostics'
	$flag_no_color_diagnostics       = '-fno-color-diagnostics'
	$flag_debug                      = '-g'
	$flag_debug_codeview             = '-gcodeview'
	$flag_define                     = '-D'
	$flag_preprocess 			     = '-E'
	$flag_include                    = '-I'
	$flag_library					 = '-l'
	$flag_library_path				 = '-L'
	$flag_link_win                   = '-Wl,'
	$flag_link_win_subsystem_console = '/SUBSYSTEM:CONSOLE'
	$flag_link_win_machine_32        = '/MACHINE:X86'
	$flag_link_win_machine_64        = '/MACHINE:X64'
	$flag_link_win_debug             = '/DEBUG'
	$flag_link_win_pdb 			     = '/PDB:'
	$flag_link_win_path_output       = '/OUT:'
	$flag_no_optimization 		     = '-O0'
	$flag_path_output                = '-o'
	$flag_preprocess_non_intergrated = '-no-integrated-cpp'
	$flag_profiling_debug            = '-fdebug-info-for-profiling'
	$flag_target_arch				 = '-target'
	$flag_wall 					     = '-Wall'
	$flag_warning 					 = '-W'
	$flag_warning_as_error 		     = '-Werror'
	$flag_win_nologo 			     = '/nologo'

	$ignore_warning_ms_include = 'no-microsoft-include'

	$target_arch = Get-TargetArchClang

	$warning_ignores = @(
		$ignore_warning_ms_include
	)

	# https://learn.microsoft.com/en-us/cpp/c-runtime-library/crt-library-features?view=msvc-170
	$libraries = @(
		'Kernel32' # For Windows API
		# 'msvcrt', # For the C Runtime (Dynamically Linked)
		# 'libucrt',
		'libcmt'    # For the C Runtime (Static Linkage)
	)

	function build-simple
	{
		param( $includes, $unit, $executable )
		Write-Host "build-simple: clang"

		$object = $executable -replace '\.exe', '.obj'
		$pdb    = $executable -replace '\.exe', '.pdb'

		$compiler_args = @(
			$flag_no_color_diagnostics,
			$flag_target_arch, $target_arch,
			$flag_wall,
			$flag_preprocess_non_intergrated,
			( $flag_define + 'GEN_TIME' ),
			( $flag_path_output + $object ),
			( $flag_include + $includes )
		)
		if ( $release -eq $false ) {
			$compiler_args += ( $flag_define + 'Build_Debug' )
			$compiler_args += $flag_debug, $flag_debug_codeview, $flag_profiling_debug
			$compiler_args += $flag_no_optimization
		}

		$warning_ignores | ForEach-Object {
			$compiler_args += $flag_warning + $_
		}

		# $compiler_args += $flag_preprocess

		$compiler_args += $flag_compile, $unit
		run-compiler $compiler $unit $compiler_args

		$linker_args = @(
			$flag_link_win_subsystem_console,
			$flag_link_win_machine_64,
			$( $flag_link_win_path_output + $executable )
		)
		if ( $release -eq $false ) {
			$linker_args += $flag_link_win_debug
			$linker_args += $flag_link_win_pdb + $pdb
		}
		else {
		}

		$libraries | ForEach-Object {
			$linker_args += $_ + '.lib'
		}

		$linker_args += $object
		run-linker $linker $executable $linker_args
	}

	$compiler = 'clang++'
	$linker   = 'lld-link'
}

if ( $vendor -match "msvc" )
{
	# https://learn.microsoft.com/en-us/cpp/build/reference/compiler-options-listed-by-category?view=msvc-170
	$flag_compile			      = '/c'
	$flag_debug                   = '/Zi'
	$flag_define		          = '/D'
	$flag_include                 = '/I'
	$flag_full_src_path           = '/FC'
	$flag_nologo                  = '/nologo'
	$flag_dll 				      = '/LD'
	$flag_dll_debug 			  = '/LDd'
	$flag_linker 		          = '/link'
	$flag_link_debug 			  = '/DEBUG'
	$flag_link_pdb 				  = '/PDB:'
	$flag_link_machine_32         = '/MACHINE:X86'
	$flag_link_machine_64         = '/MACHINE:X64'
	$flag_link_path_output 	      = '/OUT:'
	$flag_link_rt_dll             = '/MD'
	$flag_link_rt_dll_debug       = '/MDd'
	$flag_link_rt_static          = '/MT'
	$flag_link_rt_static_debug    = '/MTd'
	$flag_link_subsystem_console  = '/SUBSYSTEM:CONSOLE'
	$flag_link_subsystem_windows  = '/SUBSYSTEM:WINDOWS'
	$flag_no_optimization		  = '/Od'
	$flag_out_name                = '/OUT:'
	$flag_path_interm             = '/Fo'
	$flag_path_debug              = '/Fd'
	$flag_path_output             = '/Fe'
	$flag_preprocess_conform      = '/Zc:preprocessor'

	# This works because this project uses a single unit to build
	function build-simple
	{
		param( $includes, $unit, $executable )
		Write-Host "build-simple: msvc"

		$object = $executable -replace '\.exe', '.obj'
		$pdb    = $executable -replace '\.exe', '.pdb'

		$compiler_args = @(
			$flag_nologo,
			$flag_preprocess_conform,
			$flag_debug,
			( $flag_define + 'GEN_TIME' ),
			$flag_full_src_path,
			( $flag_path_interm + $path_build + '\' ),
			( $flag_path_output + $path_build + '\' )
		)
		if ( $release -eq $false ) {
			$compiler_args += ( $flag_define + 'Build_Debug' )
			$compiler_args += ( $flag_path_debug + $path_build + '\' )
			$compiler_args += $flag_link_rt_static_debug
			$compiler_args += $flag_no_optimization
		}
		else {
			$compiler_args += $flag_link_rt_static
		}
		$compiler_args += $includes | ForEach-Object { $flag_include + $_ }
		$compiler_args += $flag_compile, $unit
		run-compiler $compiler $unit $compiler_args

		$linker_args = @(
			$flag_nologo,
			$flag_link_machine_64,
			$flag_link_subsystem_console,
			( $flag_link_path_output + $executable )
		)
		if ( $release -eq $false ) {
			$linker_args += $flag_link_debug
			$linker_args += $flag_link_pdb + $pdb
		}
		else {
		}

		$linker_args += $object
		run-linker $linker $executable $linker_args
	}

	$compiler = 'cl'
	$linker   = 'link'
}
#endregion Configuration

#region Building
$path_build        = Join-Path $path_root build
$path_project      = Join-Path $path_root project
$path_scripts      = Join-Path $path_root scripts
$path_singleheader = Join-Path $path_root singleheader
$path_test         = Join-Path $path_root test

if ( $bootstrap )
{
	$path_build    = join-path $path_project build
	$path_gen      = join-path $path_project gen
	$path_comp_gen = join-path $path_project components/gen

	if ( -not(Test-Path($path_build) )) {
		New-Item -ItemType Directory -Path $path_build
	}
	if ( -not(Test-Path($path_gen) )) {
		New-Item -ItemType Directory -Path $path_gen
	}
	if ( -not(Test-Path($path_comp_gen) )) {
		New-Item -ItemType Directory -Path $path_comp_gen
	}

	$includes   = @( $path_project)
	$unit       = join-path $path_project "bootstrap.cpp"
	$executable = join-path $path_build   "bootstrap.exe"

	build-simple $includes $unit $executable

	Push-Location $path_project
		if ( Test-Path( $executable ) ) {
			write-host "`nRunning bootstrap"
			$time_taken = Measure-Command { & $executable
					| ForEach-Object {
						write-host `t $_ -ForegroundColor Green
					}
				}
			write-host "`nBootstrap completed in $($time_taken.TotalMilliseconds) ms"
		}
	Pop-Location
}

if ( $singleheader )
{
	$path_build = join-path $path_singleheader build
	$path_gen   = join-path $path_singleheader gen

	if ( -not(Test-Path($path_build) )) {
		New-Item -ItemType Directory -Path $path_build
	}
	if ( -not(Test-Path($path_gen) )) {
		New-Item -ItemType Directory -Path $path_gen
	}

	$includes    = @( $path_project )
	$unit       = join-path $path_singleheader "singleheader.cpp"
	$executable = join-path $path_build        "singleheader.exe"

	build-simple $includes $unit $executable

	Push-Location $path_singleheader
		if ( Test-Path( $executable ) ) {
			write-host "`nRunning singleheader generator"
			$time_taken = Measure-Command { & $executable
					| ForEach-Object {
						write-host `t $_ -ForegroundColor Green
					}
				}
			write-host "`nSingleheader generator completed in $($time_taken.TotalMilliseconds) ms"
		}
	Pop-Location
}

if ( $test )
{
	$path_gen       = join-path $path_test gen
	$path_gen_build = join-path $path_gen  build
	$path_build     = join-path $path_test build

	if ( -not(Test-Path($path_build) )) {
		New-Item -ItemType Directory -Path $path_build
	}
	if ( -not(Test-Path($path_gen) )) {
		New-Item -ItemType Directory -Path $path_gen
	}
	if ( -not(Test-Path($path_gen_build) ))  {
		New-Item -ItemType Directory -Path $path_gen_build
	}

	$path_bootstrap = join-path $path_project gen

	$includes    = @( $path_bootstrap )
	$unit       = join-path $path_test  "test.cpp"
	$executable = join-path $path_build "test.exe"

	build-simple $includes $unit $executable

	Push-Location $path_test
		Write-Host $path_test
		if ( Test-Path( $executable ) ) {
			write-host "`nRunning test generator"
			$time_taken = Measure-Command { & $executable
					| ForEach-Object {
						write-host `t $_ -ForegroundColor Green
					}
				}
			write-host "`nTest generator completed in $($time_taken.TotalMilliseconds) ms"
		}
	Pop-Location
}
#endregion Building

#region Formatting
function format-cpp
{
	param( $path, $include, $exclude )

	# Format generated gencpp
	Write-Host "`nBeginning format"
	$formatParams = @(
		'-i'          # In-place
		'-style=file:./scripts/.clang-format'
		'-verbose'
	)

	$targetFiles = @(
		Get-ChildItem -Recurse -Path $path -Include $include -Exclude $exclude
			| Select-Object -ExpandProperty FullName
	)

	$time_taken = Measure-Command {
		clang-format $formatParams $targetFiles
	}
	Write-Host "`nFormatting complete in $($time_taken.TotalMilliseconds) ms"
}

if ( $bootstrap -and (Test-Path (Join-Path $path_project "gen/gen.hpp")) )
{
	$path_gen      = join-path $path_project gen
	$path_comp_gen = join-path $path_project components/gen
	$include  = @(
		'gen.hpp', 'gen.cpp',
		'gen.dep.hpp', 'gen.dep.cpp',
		'gen.builder.hpp', 'gen.builder.cpp'
		'gen.scanner.hpp', 'gen.scanner.cpp'
	)
	$exclude  = $null
	format-cpp $path_gen $include $exclude
	format-cpp $path_comp_gen @( 'ast_inlines.hpp', 'ecode.hpp', 'especifier.hpp', 'eoperator.hpp', 'etoktype.cpp' ) $null
}

if ( $singleheader -and (Test-Path (Join-Path $path_singleheader "gen/gen.hpp")) )
{
	$path_gen = join-path $path_singleheader gen
	$include  = @(
		'gen.hpp'
	)
	$exclude  = $null
	format-cpp $path_gen $include $exclude
}

if ( $test -and $false )
{
	$path_gen = join-path $path_test gen
	$include  = @(
		'*.gen.hpp'
	)
	$exclude  = $null
	format-cpp $path_gen $include $exclude
}
#endregion Formatting

Pop-Location # $path_root
