# This build script was written to build on windows, however I did setup some generalization to allow for cross platform building.
# It will most likely need a partial rewrite to segment the build process into separate script invocations based on the OS.
# That or just rewrite it in an sh script and call it a day.

Import-Module ./helpers/target_arch.psm1
$devshell = Join-Path $PSScriptRoot 'helpers/devshell.ps1'

cls

#region Arguments
       $compiler     = $null
[bool] $release      = $false
[bool] $bootstrap    = $false
[bool] $singleheader = $false
[bool] $tests        = $false

[array] $compilers = @( "clang", "msvc" )

# This is a really lazy way of parsing the args, could use actual params down the line...

if ( $args ) { $args | ForEach-Object {
	switch ($_){
		{ $_ -in $compilers } { $compiler     = $_; break }
		"release"             { $release      = $true }
		"bootstrap"           { $bootstrap    = $true }
		"singleheader"        { $singleheader = $true }
		"tests"               { $tests        = $true }
	}
}}
#endregion Arguments

#region Building
write-host "Building gencpp with $compiler"

if ($IsWindows) {
	# This library was really designed to only run on 64-bit systems.
	# (Its a development tool after all)
    & $devshell -arch amd64
}

$path_root         = git rev-parse --show-toplevel
$path_build        = Join-Path $path_root build
$path_project      = Join-Path $path_root project
$path_scripts      = Join-Path $path_root scripts
$path_singleheader = Join-Path $path_root singleheader
$path_test         = Join-Path $path_root test

write-host "Build Type: $(if ($release) {"Release"} else {"Debug"} )"

if ( $compiler -eq $null ) {
	write-host "No compilier specified, assuming clang available"
	$compiler = "clang"
}

Push-Location $path_root

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

if ( $compiler -match "clang" )
{
	# https://clang.llvm.org/docs/ClangCommandLineReference.html
	$flag_compile                    = '-c'
	$flag_color_diagnostics          = '-fcolor-diagnostics'
	$flag_no_color_diagnostics       = '-fno-color-diagnostics'
	$flag_debug                      = '-g'
	$flag_debug_codeview             = '-gcodeview'
	$flag_define                     = '-D'
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

	if ( $bootstrap )
	{
		$path_build = join-path $path_project build
		$path_gen   = join-path $path_project gen

		if ( -not(Test-Path($path_build) )) {
			New-Item -ItemType Directory -Path $path_build
		}
		if ( -not(Test-Path($path_gen) )) {
			New-Item -ItemType Directory -Path $path_gen
		}

		$include    = $path_project
		$unit       = join-path $path_project "bootstrap.cpp"
		$object     = join-path $path_build   "bootstrap.obj"
		$executable = join-path $path_build   "bootstrap.exe"
		$pdb 	    = join-path $path_build   "bootstrap.pdb"

		$compiler_args = @(
			$flag_no_color_diagnostics,
			$flag_target_arch, $target_arch,
			$flag_wall,
			$flag_preprocess_non_intergrated,
			( $flag_define + 'GEN_TIME' ),
			( $flag_path_output + $object ),
			( $flag_include + $include )
		)

		$warning_ignores | ForEach-Object {
			$compiler_args += $flag_warning + $_
		}

		if ( $release -eq $false ) {
			$compiler_args += ( $flag_define + 'Build_Debug' )
			$compiler_args += $flag_debug, $flag_debug_codeview, $flag_profiling_debug
		}

		$compiler_args += $flag_compile, $unit
	    run-compiler clang++ $unit $compiler_args

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
		run-linker lld-link $executable $linker_args

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

		$include    = $path_project
		$unit       = join-path $path_singleheader "singleheader.cpp"
		$object	    = join-path $path_build        "singleheader.obj"
		$executable = join-path $path_build        "singleheader.exe"
		$pdb 	    = join-path $path_build        "singleheader.pdb"

		$compiler_args = @(
			$flag_no_color_diagnostics,
			$flag_target_arch, $target_arch,
			$flag_wall,
			$flag_preprocess_non_intergrated,
			( $flag_define + 'GEN_TIME' ),
			( $flag_path_output + $object ),
			( $flag_include + $include )
		)
		if ( $release -eq $false ) {
			$compiler_args += ( $flag_define + 'Build_Debug' )
			$compiler_args += $flag_debug, $flag_debug_codeview, $flag_profiling_debug
		}

		$warning_ignores | ForEach-Object {
			$compiler_args += $flag_warning + $_
		}

		$compiler_args += $flag_compile, $unit
		run-compiler clang $unit $compiler_args

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
		run-linker lld-link $executable $linker_args

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
	    # ... [your test compilation code here]
	}
}

if ( $compiler -match "msvc" )
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
	$flag_out_name                = '/OUT:'
	$flag_path_interm             = '/Fo'
	$flag_path_debug              = '/Fd'
	$flag_path_output             = '/Fe'
	$flag_preprocess_conform      = '/Zc:preprocessor'

	[array] $compiler_args = $null
	[array] $linker_args   = $null

	if ( $bootstrap )
	{
		$path_build = join-path $path_project build
		$path_gen   = join-path $path_project gen

		if ( -not(Test-Path($path_build) )) {
			New-Item -ItemType Directory -Path $path_build
		}
		if ( -not(Test-Path($path_gen) )) {
			New-Item -ItemType Directory -Path $path_gen
		}

		$include    = $path_project
		$unit       = join-path $path_project "bootstrap.cpp"
		$object     = join-path $path_build   "bootstrap.obj"
		$executable = join-path $path_build   "bootstrap.exe"
		$pdb 	    = join-path $path_build   "bootstrap.pdb"

		$compiler_args = @(
			$flag_nologo,
			$flag_preprocess_conform,
			$flag_define + 'GEN_TIME',
			$flag_full_src_path,
			( $flag_path_interm + $path_build + '\' ),
			( $flag_path_output + $path_build + '\' ),
			( $flag_include + $include )
		)

		if ( $release -eq $false ) {
			$compiler_args += ( $flag_define + 'Build_Debug' )
			$compiler_args += $flag_debug
			$compiler_args += ( $flag_path_debug + $path_build + '\' )
			$compiler_args += $flag_link_rt_static_debug
		}
		else {
			$compiler_args += $flag_link_rt_static
		}

		$compiler_args += $flag_compile, $unit
		run-compiler cl $unit $compiler_args

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
		run-linker link $executable $linker_args

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

		$include    = $path_project
		$unit       = join-path $path_singleheader "singleheader.cpp"
		$object     = join-path $path_build        "singleheader.obj"
		$executable = join-path $path_build        "singleheader.exe"
		$pdb 	    = join-path $path_build        "singleheader.pdb"

		$compiler_args = @(
			$flag_nologo,
			$flag_preprocess_conform,
			$flag_debug,
			( $flag_define + 'GEN_TIME' ),
			$flag_full_src_path,
			( $flag_path_interm + $path_build + '\' ),
			( $flag_path_output + $path_build + '\' ),
			( $flag_include + $include )
		)

		if ( $release -eq $false ) {
			$compiler_args += ( $flag_define + 'Build_Debug' )
			$compiler_args += ( $flag_path_debug + $path_build + '\' )
			$compiler_args += $flag_link_rt_static_debug
		}
		else {
			$compiler_args += $flag_link_rt_static
		}

		$compiler_args += $flag_compile, $unit
		run-compiler cl $unit $compiler_args

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
		run-linker link $executable $linker_args

		Push-Location $path_singleheader
			if ( Test-Path($executable) ) {
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

	}
}
#endregion Building

#region Formatting
if ( $bootstrap -and (Test-Path (Join-Path $path_project "gen/gen.hpp")) )
{
	$path_gen = join-path $path_project gen

	# Format generated gencpp
	Write-Host "`nBeginning format"
	$formatParams = @(
		'-i'          # In-place
		'-style=file:./scripts/.clang-format'
		'-verbose'
	)

	$include = @(
		'gen.hpp', 'gen.cpp',
		'gen.dep.hpp', 'gen.dep.cpp',
		'gen.builder.hpp', 'gen.builder.cpp'
		'gen.scanner.hpp', 'gen.scanner.cpp'
	)
	$exclude = $null

	$targetFiles = @(Get-ChildItem -Recurse -Path $path_gen -Include $include -Exclude $exclude | Select-Object -ExpandProperty FullName)

	$time_taken = Measure-Command {
		clang-format $formatParams $targetFiles
	}
	Write-Host "`nFormatting complete in $($time_taken.TotalMilliseconds) ms"
}

if ( $singleheader -and (Test-Path (Join-Path $path_singleheader "gen/gen.hpp")) )
{
	$path_gen = join-path $path_singleheader gen

	# Format generated gencpp
	Write-Host "`nBeginning format"
	$formatParams = @(
		'-i'          # In-place
		'-style=file:./scripts/.clang-format'
		'-verbose'
	)

	$include = @(
		'gen.hpp'
	)
	$exclude = $null

	$targetFiles = @(Get-ChildItem -Recurse -Path $path_gen -Include $include -Exclude $exclude | Select-Object -ExpandProperty FullName)

	$time_taken = Measure-Command {
		clang-format $formatParams $targetFiles
	}
	Write-Host "`nFormatting complete in $($time_taken.TotalMilliseconds) ms"
}

if ( $test )
{

}
#endregion Formatting

Pop-Location # $path_root
