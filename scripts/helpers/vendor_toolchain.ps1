# This is meant to be used with build.ps1, and is not a standalone script.
  $target_arch = Join-Path $PSScriptRoot 'target_arch.psm1'

  import-module $target_arch

if ($IsWindows) {
	# This HandmadeHero implementation is only designed for 64-bit systems
    & $devshell -arch amd64
}

if ( $vendor -eq $null ) {
	write-host "No vendor specified, assuming clang available"
	$vendor = "clang"
}

if ( $dev ) {
	if ( $debug -eq $null ) {
		$debug = $true
	}
	if ( $optimize -eq $null ) {
		$optimize = $false
	}
}

# Add new function for running lib.exe
function run-archiver
{
    param( $archiver, $library, $lib_args )

    write-host "`Creating library $library"
    if ( $verbose ) {
        write-host "Lib manager config:"
        $lib_args | ForEach-Object {
            write-host $_ -ForegroundColor Cyan
        }
    }

    $time_taken = Measure-Command {
        & $archiver $lib_args 2>&1 | ForEach-Object {
            $color = 'White'
            switch ($_){
                { $_ -match "error"   } { $color = 'Red'   ; break }
                { $_ -match "warning" } { $color = 'Yellow'; break }
            }
            write-host `t $_ -ForegroundColor $color
        }
    }

    if ( $LASTEXITCODE -eq 0 ) {
        write-host "$library creation finished in $($time_taken.TotalMilliseconds) ms`n"
        return $true
    }
    else {
        write-host "Library creation failed for $library`n" -ForegroundColor Red
        return $false
    }
}

function run-compiler
{
	param( $compiler, $unit, $compiler_args )

	if ( $analysis ) {
		$compiler_args += $flag_syntax_only
	}

	write-host "`Compiling $unit"
	if ( $verbose ) {
		write-host "Compiler config:"
		$compiler_args | ForEach-Object {
			write-host $_ -ForegroundColor Cyan
		}
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

	if ( $LASTEXITCODE -eq 0 ) {
		write-host "$unit compile finished in $($time_taken.TotalMilliseconds) ms`n"
		return $true
	}
	else {
		write-host "Compile failed for $unit`n" -ForegroundColor Red
		return $false
	}
}

function run-linker
{
	param( $linker, $binary, $linker_args )

	write-host "`Linking $binary"
	if ( $verbose ) {
		write-host "Linker config:"
		$linker_args | ForEach-Object {
			write-host $_ -ForegroundColor Cyan
		}
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

	if ( $LASTEXITCODE -eq 0 ) {
		write-host "$binary linking finished in $($time_taken.TotalMilliseconds) ms`n"
		return $true
	}
	else {
		write-host "Linking failed for $binary`n" -ForegroundColor Red
		return $false
	}
}

if ( $vendor -match "clang" )
{
	# https://clang.llvm.org/docs/ClangCommandLineReference.html
	$flag_all_c 					   = @('-x', 'c')
	$flag_c11                          = '-std=c11'
	$flag_all_cpp                      = '-x c++'
	$flag_compile                      = '-c'
	$flag_color_diagnostics            = '-fcolor-diagnostics'
	$flag_no_color_diagnostics         = '-fno-color-diagnostics'
	$flag_debug                        = '-g'
	$flag_debug_codeview               = '-gcodeview'
	$flag_define                       = '-D'
	$flag_exceptions_disabled		   = '-fno-exceptions'
	$flag_preprocess 			       = '-E'
	$flag_include                      = '-I'
	$flag_section_data                 = '-fdata-sections'
	$flag_section_functions            = '-ffunction-sections'
	$flag_library					   = '-l'
	$flag_library_path				   = '-L'
	$flag_linker                       = '-Wl,'
	if ( $IsWindows ) {
		$flag_link_dll                 = '/DLL'
		$flag_link_mapfile 		       = '/MAP:'
		$flag_link_optimize_references = '/OPT:REF'
	}
	if ( $IsLinux ) {
		$flag_link_mapfile              = '--Map='
		$flag_link_optimize_references  = '--gc-sections'
	}
	$flag_link_win_subsystem_console    = '/SUBSYSTEM:CONSOLE'
	$flag_link_win_subsystem_windows    = '/SUBSYSTEM:WINDOWS'
	$flag_link_win_machine_32           = '/MACHINE:X86'
	$flag_link_win_machine_64           = '/MACHINE:X64'
	$flag_link_win_debug                = '/DEBUG'
	$flag_link_win_pdb 			        = '/PDB:'
	$flag_link_win_path_output          = '/OUT:'
	$flag_no_optimization 		        = '-O0'
	$flag_optimize_fast 		        = '-O2'
	$flag_optimize_size 		        = '-O1'
	$flag_optimize_intrinsics		    = '-Oi'
	$flag_path_output                   = '-o'
	$flag_preprocess_non_intergrated    = '-no-integrated-cpp'
	$flag_profiling_debug               = '-fdebug-info-for-profiling'
	$flag_set_stack_size			    = '-stack='
	$flag_syntax_only				    = '-fsyntax-only'
	$flag_target_arch				    = '-target'
	$flag_time_trace					= '-ftime-trace'
	$flag_verbose 					    = '-v'
	$flag_wall 					        = '-Wall'
	$flag_warning 					    = '-W'
	$flag_warnings_as_errors            = '-Werror'
	$flag_win_nologo 			        = '/nologo'

	$ignore_warning_ms_include            = 'no-microsoft-include'
	$ignore_warning_return_type_c_linkage = 'no-return-type-c-linkage'

	$target_arch = Get-TargetArchClang

	$warning_ignores = @(
		$ignore_warning_ms_include,
		$ignore_warning_return_type_c_linkage
	)

	# https://learn.microsoft.com/en-us/cpp/c-runtime-library/crt-library-features?view=msvc-170
	if ( $IsWindows ) {
		$libraries = @(
			'Kernel32' # For Windows API
			# 'msvcrt', # For the C Runtime (Dynamically Linked)
			# 'libucrt',
			'libcmt'    # For the C Runtime (Static Linkage)
		)
	}

	function build
	{
		param( [string]$path_output, [array]$includes, [array]$compiler_args, [array]$linker_args, [array]$units, [string]$binary )
		$result = $false
		#Write-Host "build: clang"

		$map    = $binary -replace '\.(exe|dll)$', '.map'
		$map    = join-path $path_output (split-path $map    -Leaf)

		# This allows dll reloads at runtime to work (jankily, use below if not interested)
		# $pdb    = $binary -replace '\.(exe|dll)$', "_$(get-random).pdb"
		$pdb    = $binary -replace '\.(exe|dll)$', ".pdb"

		$compiler_args += @(
			$flag_no_color_diagnostics,
			$flag_exceptions_disabled,
			$flag_target_arch, $target_arch,
			$flag_wall,
			$flag_preprocess_non_intergrated
			# $flag_section_data,
			# $flag_section_functions,
		)
		if ( $verbose ) {
			# $compiler_args += $flag_verbose
			# $compiler_args += $flag_time_trace
		}
		if ( $optimize ) {
			$compiler_args += $flag_optimize_size
		}
		else {
			$compiler_args += $flag_no_optimization
		}
		if ( $debug ) {
			$compiler_args += ( $flag_define + 'GEN_BUILD_DEBUG=1' )
			$compiler_args += $flag_debug, $flag_debug_codeview, $flag_profiling_debug
		}
		else {
			$compiler_args += ( $flag_define + 'GEN_BUILD_DEBUG=0' )
		}

		$warning_ignores | ForEach-Object {
			$compiler_args += $flag_warning + $_
		}
		$compiler_args += $includes | ForEach-Object { $flag_include + $_ }

		$objects = @()
		foreach ( $unit in $units )
		{
			$object = $unit -replace '\.(cpp|c)$', '.obj'
			$object = join-path $path_output (split-path $object -Leaf)
			$objects += $object

			$unit_compiler_args  = $compiler_args
			$unit_compiler_args += ( $flag_path_output + $object )

			$unit_compiler_args += $flag_compile, $unit
			run-compiler $compiler $unit $unit_compiler_args
		}

		$linker_args += @(
			$flag_link_win_machine_64,
			$( $flag_link_win_path_output + $binary )
		)
		if ( $debug ) {
			$linker_args += $flag_link_win_debug
			$linker_args += $flag_link_win_pdb + $pdb
			$linker_args += $flag_link_mapfile + $map
		}

		$libraries | ForEach-Object {
			$linker_args += $_ + '.lib'
		}

		$linker_args += $objects
		return run-linker $linker $binary $linker_args
	}

	function build-simple
	{
		param( [string]$path_output, [array]$includes, [array]$compiler_args, [array]$linker_args, [string]$unit, [string]$binary )
		$result = $false
		#Write-Host "build-simple: clang"

		$object = $unit -replace '\.(cpp|c)$', '.obj'
		$map    = $unit -replace '\.(cpp|c)$', '.map'
		$object = join-path $path_output (split-path $object -Leaf)
		$map    = join-path $path_output (split-path $map    -Leaf)

		# This allows dll reloads at runtime to work (jankily, use below if not interested)
		# $pdb    = $binary -replace '\.(exe|dll)$', "_$(get-random).pdb"
		$pdb    = $binary -replace '\.(exe|dll)$', ".pdb"

		$compiler_args += @(
			$flag_no_color_diagnostics,
			$flag_exceptions_disabled,
			$flag_target_arch, $target_arch,
			$flag_wall,
			$flag_preprocess_non_intergrated,
			# $flag_section_data,
			# $flag_section_functions,
			( $flag_path_output + $object )
		)
		if ( $verbose ) {
			# $compiler_args += $flag_verbose
			# $compiler_args += $flag_time_trace
		}
		if ( $optimize ) {
			$compiler_args += $flag_optimize_size
		}
		else {
			$compiler_args += $flag_no_optimization
		}
		if ( $debug ) {
			$compiler_args += ( $flag_define + 'GEN_BUILD_DEBUG=1' )
			$compiler_args += $flag_debug, $flag_debug_codeview, $flag_profiling_debug
		}
		else {
			$compiler_args += ( $flag_define + 'GEN_BUILD_DEBUG=0' )
		}

		$warning_ignores | ForEach-Object {
			$compiler_args += $flag_warning + $_
		}
		$compiler_args += $includes | ForEach-Object { $flag_include + $_ }

		$compiler_args += $flag_compile, $unit
		if ( (run-compiler $compiler $unit $compiler_args) -eq $false ) {
			return $false
		}

		$linker_args += @(
			$flag_link_win_machine_64,
			$( $flag_link_win_path_output + $binary )
		)
		if ( $debug ) {
			$linker_args += $flag_link_win_debug
			$linker_args += $flag_link_win_pdb + $pdb
			$linker_args += $flag_link_mapfile + $map
		}

		$libraries | ForEach-Object {
			$linker_args += $_ + '.lib'
		}

		# Check if output is a static library
		if ( $binary -match '\.lib$' )
		{
			$lib_args  = @()
			# $lib_args += $flag_nologo
			# $lib_args += $flag_link_win_machine_64
			# $lib_args += ( $flag_link_win_path_output + $binary )
			# $lib_args += '--format=windows'
			# $lib_args += '-X64'
			$lib_args += $object
			return run-archiver $archiver $binary $lib_args
		}

		$linker_args += $object
		return run-linker $linker $binary $linker_args
	}

	$archiver = 'llvm-ar'
	$compiler = 'clang++'
	$linker   = 'lld-link'
}

if ( $vendor -match "msvc" )
{
	# https://learn.microsoft.com/en-us/cpp/build/reference/compiler-options-listed-by-category?view=msvc-170
	$flag_all_c 					  = '/TC'
	$flag_c11                         = '/std:c11'
	$flag_all_cpp                     = '/TP'
	$flag_compile			          = '/c'
	$flag_debug                       = '/Zi'
	$flag_define		              = '/D'
	$flag_exceptions_disabled		  = '/EHsc-'
	$flag_RTTI_disabled				  = '/GR-'
	$flag_include                     = '/I'
	$flag_full_src_path               = '/FC'
	$flag_nologo                      = '/nologo'
	$flag_dll 				          = '/LD'
	$flag_dll_debug 			      = '/LDd'
	$flag_linker 		              = '/link'
	# $flag_link_lib                    = '/lib'
	$flag_link_dll                    = '/DLL'
	$flag_link_no_incremental 	      = '/INCREMENTAL:NO'
	$flag_link_mapfile 				  = '/MAP:'
	$flag_link_optimize_references    = '/OPT:REF'
	$flag_link_win_debug 	          = '/DEBUG'
	$flag_link_win_pdb 		          = '/PDB:'
	$flag_link_win_machine_32         = '/MACHINE:X86'
	$flag_link_win_machine_64         = '/MACHINE:X64'
	$flag_link_win_path_output        = '/OUT:'
	$flag_link_win_rt_dll             = '/MD'
	$flag_link_win_rt_dll_debug       = '/MDd'
	$flag_link_win_rt_static          = '/MT'
	$flag_link_win_rt_static_debug    = '/MTd'
	$flag_link_win_subsystem_console  = '/SUBSYSTEM:CONSOLE'
	$flag_link_win_subsystem_windows  = '/SUBSYSTEM:WINDOWS'
	$flag_no_optimization		      = '/Od'
	$flag_optimize_fast 		      = '/O2'
	$flag_optimize_size 		      = '/O1'
	$flag_optimize_intrinsics		  = '/Oi'
	$flag_optimized_debug_forceinline = '/d2Obforceinline'
	$flag_optimized_debug			  = '/Zo'
	$flag_
	# $flag_out_name                    = '/OUT:'
	$flag_path_interm                 = '/Fo'
	$flag_path_debug                  = '/Fd'
	$flag_path_output                 = '/Fe'
	$flag_preprocess_conform          = '/Zc:preprocessor'
	$flag_updated_cpp_macro           = "/Zc:__cplusplus"
	$flag_set_stack_size			  = '/F'
	$flag_syntax_only				  = '/Zs'
	$flag_wall 					      = '/Wall'
	$flag_warnings_as_errors 		  = '/WX'
	$flag_lib_list                    = '/LIST'

	function build
	{
		param( [string]$path_output, [array]$includes, [array]$compiler_args, [array]$linker_args, [array]$units, [string]$binary )
		$result = $false
		#Write-Host "build-simple: msvc"

		$map    = $binary -replace '\.(exe|dll)$', '.map'
		$map    = join-path $path_output (split-path $map    -Leaf)

		# This allows dll reloads at runtime to work (jankily, use below if not interested)
		# $pdb    = $binary -replace '\.(exe|dll)$', "_$(get-random).pdb"
		$pdb    = $binary -replace '\.(exe|dll)$', ".pdb"

		$compiler_args += @(
			$flag_nologo,
			# $flag_all_cpp,
			$flag_exceptions_disabled,
			( $flag_define + '_HAS_EXCEPTIONS=0' ),
			$flag_RTTI_disabled,
			$flag_preprocess_conform,
			$flag_full_src_path,
			( $flag_path_interm + $path_output + '\' ),
			( $flag_path_output + $path_output + '\' )
		)

		if ( $verbose ) {
		}

		if ( $optimize ) {
			$compiler_args += $flag_optimize_size
		}
		else {
			$compiler_args += $flag_no_optimization
		}

		if ( $debug )
		{
			$compiler_args += $flag_debug
			$compiler_args += ( $flag_define + 'GEN_BUILD_DEBUG=1' )
			$compiler_args += ( $flag_path_debug + $path_output + '\' )
			$compiler_args += $flag_link_win_rt_static_debug

			if ( $optimize ) {
				$compiler_args += $flag_optimized_debug
				$compiler_args += $flag_optimized_debug_forceinline
			}
		}
		else {
			$compiler_args += ( $flag_define + 'GEN_BUILD_DEBUG=0' )
			$compiler_args += $flag_link_win_rt_static
		}
		$compiler_args += $includes | ForEach-Object { $flag_include + $_ }

		$objects = @()
		foreach ( $unit in $units )
		{
			$object   = $unit -replace '\.(cpp|c)$', '.obj'
			$object   = join-path $path_output (split-path $object -Leaf)
			$objects += $object

			$unit_compiler_args  = $compiler_args
			# $unit_compiler_args += ( $flag_path_output + $object )

			$unit_compiler_args += $flag_compile, $unit
			run-compiler $compiler $unit $unit_compiler_args
		}

		$linker_args += @(
			$flag_nologo,
			$flag_link_win_machine_64,
			$flag_link_no_incremental,
			( $flag_link_win_path_output + $binary )
		)
		if ( $debug ) {
			$linker_args += $flag_link_win_debug
			$linker_args += $flag_link_win_pdb + $pdb
			$linker_args += $flag_link_mapfile + $map
		}
		else {
		}

		$linker_args += $objects
		return run-linker $linker $binary $linker_args
	}

	function build-simple
	{
		param( [string]$path_output, [array]$includes, [array]$compiler_args, [array]$linker_args, [string]$unit, [string]$binary )
		$result = $false
		#Write-Host "build-simple: msvc"

		$object = $unit -replace '\.(cpp|c)$', '.obj'
		$map    = $unit -replace '\.(cpp|c)$', '.map'
		$object = join-path $path_output (split-path $object -Leaf)
		$map    = join-path $path_output (split-path $map    -Leaf)

		# This allows dll reloads at runtime to work (jankily, use below if not interested)
		# $pdb    = $binary -replace '\.(exe|dll)$', "_$(get-random).pdb"
		$pdb    = $binary -replace '\.(exe|dll)$', ".pdb"

		$compiler_args += @(
			$flag_nologo,
			# $flag_all_cpp,
			$flag_exceptions_disabled,
			( $flag_define + '_HAS_EXCEPTIONS=0' ),
			$flag_RTTI_disabled,
			$flag_preprocess_conform,
			$flag_full_src_path,
			( $flag_path_interm + $path_output + '\' ),
			( $flag_path_output + $path_output + '\' )
		)

		if ( $verbose ) {
		}

		if ( $optimize ) {
			$compiler_args += $flag_optimize_size
		}
		else {
			$compiler_args += $flag_no_optimization
		}

		if ( $debug )
		{
			$compiler_args += $flag_debug
			$compiler_args += ( $flag_define + 'GEN_BUILD_DEBUG=1' )
			$compiler_args += ( $flag_path_debug + $path_output + '\' )
			$compiler_args += $flag_link_win_rt_static_debug

			if ( $optimize ) {
				$compiler_args += $flag_optimized_debug
			}
		}
		else {
			$compiler_args += ( $flag_define + 'GEN_BUILD_DEBUG=0' )
			$compiler_args += $flag_link_win_rt_static
		}
		$compiler_args += $includes | ForEach-Object { $flag_include + $_ }

		$compiler_args += $flag_compile, $unit
		if ( (run-compiler $compiler $unit $compiler_args) -eq $false ) {
			return $false;
		}

		# Check if output is a static library
		if ( $binary -match '\.lib$' )
		{
			$lib_args  = @()
			$lib_args += $flag_nologo
			$lib_args += $flag_link_win_machine_64
			$lib_args += ( $flag_link_win_path_output + $binary )
			$lib_args += $object
			return run-archiver $archiver $binary $lib_args
		}

		$linker_args += @(
			$flag_nologo,
			$flag_link_win_machine_64,
			$flag_link_no_incremental,
			( $flag_link_win_path_output + $binary )
		)
		if ( $debug ) {
			$linker_args += $flag_link_win_debug
			$linker_args += $flag_link_win_pdb + $pdb
			$linker_args += $flag_link_mapfile + $map
		}
		else {
		}

		# Check if output is a dynamic library
		if ( $binary -match '\.dll$' ) {
			$linker_args += $flag_link_dll
		}
		$linker_args += $object
		# Write-Host "link args:"
		# $linker_args | ForEach-Object {
		# 	Write-Host "`t$_" -ForegroundColor Yellow
		# }
		return run-linker $linker $binary $linker_args
	}

	$archiver = 'lib'
	$compiler = 'cl'
	$linker   = 'link'
}
