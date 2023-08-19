Import-Module ./helpers/target_arch.psm1

cls

#region Arguments
         $compiler     = $null
[bool]   $release      = $false
[bool]   $bootstrap    = $false
[bool]   $singleheader = $false
[bool]   $tests        = $false

[array] $compilers = @( "clang", "gcc", "msvc" )

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

Invoke-Expression "& $(join-path $PSScriptRoot 'helpers/devshell.ps1') -arch x64"

$path_root         = git rev-parse --show-toplevel
$path_build        = Join-Path $path_root    build
$path_project      = Join-Path $path_root    project
$path_scripts      = Join-Path $path_root    scripts
$path_singleheader = Join-Path $path_root    singleheader
$path_test         = Join-Path $path_root    test

write-host "Build Type: $(if ($release) {"Release"} else {"Debug"} )"

if ( $compiler -eq $null ) {
	write-host "No compilier specified, assuming clang available"
	$compiler = "clang"
}

Push-Location $path_root

function run-compiler
{
	param( $compiler, $executable, $path_build, $path_gen, $compiler_args )

	write-host "`nBuilding $executable"
	write-host "Compiler config:"
	$compiler_args | ForEach-Object {
		write-host $_ -ForegroundColor Cyan
	}

	if ( -not(Test-Path($path_build) )) {
		New-Item -ItemType Directory -Path $path_build
	}
	if ( -not(Test-Path($path_gen) )) {
		New-Item -ItemType Directory -Path $path_gen
	}

	$time_taken = Measure-Command {
		& $compiler $compiler_args
			| ForEach-Object {
				$color = 'White'
				switch ($_){
					{ $_ -match "error"   } { $color = 'Red'   ; break }
					{ $_ -match "warning" } { $color = 'Yellow'; break }
				}
				write-host `t $_ -ForegroundColor $color
			}
	}
	write-host "$executable built in $($time_taken.TotalMilliseconds) ms"
}

if ( $compiler -match "clang" )
{
	$target_arch = Get-TargetArchClang

	$flag_compile_only               = '-c'
	$flag_debug                      = '-g'
	$flag_debug_codeview             = '-gcodeview'
	$flag_define                     = '-D'
	$flag_include                    = '-I'
	$flag_library					 = '-l'
	$flag_library_path				 = '-L'
	$flag_path_output                = '-o'
	$flag_preprocess_non_intergrated = '-no-integrated-cpp'
	$flag_profiling_debug            = '-fdebug-info-for-profiling'
	$flag_target_arch				 = '-target'
	$flag_x_linker				     = '-Xlinker'
	$flag_machine_32 			     = '/machine:X64'
	$flag_machine_64 			     = '/machine:X64'
	$flag_win_linker                 = '-Wl,'
	$flag_win_subsystem_console      = '/SUBSYSTEM:CONSOLE'
	$flag_win_machine_32			 = '/MACHINE:X86'
	$flag_win_machine_64			 = '/MACHINE:X64'

	# $library_paths = @(
	# 	'C:\Windows\System32'

	# )

	if ( $bootstrap )
	{
		$path_build = join-path $path_project build
		$path_gen   = join-path $path_project gen

		$include    = $path_project
		$unit       = join-path $path_project "bootstrap.cpp"
		$executable = join-path $path_build   "bootstrap.exe"

		$compiler_args = @(
			$flag_target_arch, $target_arch,
			$flag_preprocess_non_intergrated,
			$( $flag_define + 'GEN_TIME' ),
			$flag_path_output, $executable,
			$( $flag_include + $include )
		)
		if ( $release -eq $false ) {
			$compiler_args += $( $flag_define + 'Build_Debug' )
			$compiler_args += $flag_debug, $flag_debug_codeview, $flag_profiling_debug
		}

		$linker_args = @(
			$flag_x_linker,
			# $( $flag_linker + $flag_win_subsystem_console ),
			$( $flag_linker + $flag_machine_64 )
		)
		$libraries = @(
			'Kernel32',    # For Windows API
			# 'msvcrt',      # For the C Runtime (Dynamically Linked)
			'libucrt',
			'libcmt'    # For the C Runtime (Static Linkage)
		)
		$compiler_args += $linker_args
		$compiler_args += $libraries | ForEach-Object { $flag_library + $_ }

		$compiler_args += $unit
	    run-compiler clang $executable $path_build $path_gen $compiler_args

		Push-Location $path_project
			if ( Test-Path($executable) ) {
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

		$include    = $path_project
		$unit       = join-path $path_singleheader "singleheader.cpp"
		$executable = join-path $path_build        "singleheader.exe"

		$compiler_args = @(
			$flag_target_arch, $target_arch,
			$flag_preprocess_non_intergrated,
			$( $flag_define + 'GEN_TIME' ),
			$flag_path_output, $executable,
			$( $flag_include + $include )
		)
		if ( $release -eq $false ) {
			$compiler_args += $( $flag_define + 'Build_Debug' )
			$compiler_args += $flag_debug, $flag_debug_codeview, $flag_profiling_debug
		}

		$compiler_args += $unit
		run-compiler clang $executable $path_build $path_gen $compiler_args

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
	    # ... [your test compilation code here]
	}
}

if ( $compiler -match "msvc" )
{
	$flag_debug              = '/Zi'
	$flag_define		     = '/D'
	$flag_include            = '/I'
	$flag_full_src_path      = '/FC'
	$flag_nologo             = '/nologo'
	$flag_linker 		     = '/link'
	$flag_out_name           = '/OUT:'
	$flag_path_interm        = '/Fo'
	$flag_path_debug         = '/Fd'
	$flag_path_output        = '/Fe'
	$flag_preprocess_conform = '/Zc:preprocessor'

	[array] $compiler_args = $null

	if ( $bootstrap )
	{
		$path_build = join-path $path_project build
		$path_gen   = join-path $path_project gen

		$include    = $path_project
		$unit       = join-path $path_project "bootstrap.cpp"
		$executable = join-path $path_build   "bootstrap.exe"


		$compiler_args = @(
			$flag_nologo,
			$flag_debug,
			$flag_preprocess_conform,
			$( $flag_define + 'GEN_TIME' ),
			$flag_full_src_path,
			$( $flag_path_interm + $path_build + '\' ),
			$( $flag_path_output + $path_build + '\' ),
			$( $flag_include + $include )
		)

		if ( $release -eq $false ) {
			$compiler_args += $( $flag_define + 'Build_Debug' )
			$compiler_args += $( $flag_path_debug + $path_build + '\' )
		}

		$compiler_args += $unit
		run-compiler cl $executable $path_build $path_gen $compiler_args

		Push-Location $path_project
			if ( Test-Path($executable) ) {
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

		$include    = $path_project
		$unit       = join-path $path_singleheader "singleheader.cpp"
		$executable = join-path $path_build        "singleheader.exe"

		$compiler_args = @(
			$flag_nologo,
			$flag_debug,
			$flag_preprocess_conform,
			$( $flag_define + 'GEN_TIME' ),
			$flag_full_src_path,
			$( $flag_path_interm + $path_build + '\' ),
			$( $flag_path_output + $path_build + '\' ),
			$( $flag_include + $include )
		)

		if ( $release -eq $false ) {
			$compiler_args += $( $flag_define + 'Build_Debug' )
			$compiler_args += $( $flag_path_debug + $path_build + '\' )
		}

		$compiler_args += $unit
		run-compiler cl $executable $path_build $path_gen $compiler_args

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
#pragma endregion Building

#pragma region Formatting
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

if ( $singleheader )
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
#pragma endregion Formatting

Pop-Location # $path_root
