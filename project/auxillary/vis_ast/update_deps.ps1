Clear-Host

$path_root     = git rev-parse --show-toplevel
$path_scripts  = Join-Path $path_root 'scripts'

$target_arch        = Join-Path $path_scripts 'helpers/target_arch.psm1'
$devshell           = Join-Path $path_scripts 'helpers/devshell.ps1'
$format_cpp	        = Join-Path $path_scripts 'helpers/format_cpp.psm1'
$incremental_checks = Join-Path $path_scripts 'helpers/incremental_checks.ps1'
$vendor_toolchain   = Join-Path $path_scripts 'helpers/vendor_toolchain.ps1'

$path_project  = Join-Path $path_root     'project'
$path_aux      = Join-Path $path_project  'auxillary'
$path_vis_root = Join-Path $path_aux      'vis_ast'
$path_binaries = Join-Path $path_vis_root 'binaries'

$path_deps = Join-Path $path_vis_root 'dependencies'
$path_temp = Join-Path $path_deps     'temp'

Import-Module $target_arch
Import-Module $format_cpp

#region Arguments
$vendor           = $null
$optimize         = $null
$debug 	          = $null
$analysis	      = $false
$dev              = $false
$verbose          = $null

[array] $vendors = @( "clang", "msvc" )

# This is a really lazy way of parsing the args, could use actual params down the line...

if ( $args ) { $args | ForEach-Object {
switch ($_){
 { $_ -in $vendors }   { $vendor    = $_; break }
 "optimize"            { $optimize  = $true }
 "debug"               { $debug     = $true }
 "analysis"            { $analysis  = $true }
 "dev"                 { $dev       = $true }
 "verbose"             { $verbose   = $true }
}
}}
#endregion Argument

# Load up toolchain configuraion
. $vendor_toolchain
. $incremental_checks

# Clear out the current content first
if ( test-path $path_temp) {
	remove-item $path_temp -Recurse
}
New-Item -ItemType Directory -Path $path_temp

if ( -not (Test-Path $path_binaries) ) {
	New-Item -ItemType Directory -Path $path_binaries
}

function setup-raylib {
	$path_raylib     = join-path $path_deps   'raylib'
	$path_raylib_inc = join-path $path_raylib 'include'
	$path_raylib_lib = join-path $path_raylib 'lib'
	if ( test-path $path_raylib_inc ) {
		remove-item $path_raylib_inc -recurse
		remove-item $path_raylib_lib -recurse
	}
	new-item -path $path_raylib_inc -ItemType Directory
	new-item -path $path_raylib_lib -ItemType Directory

	$url_raylib_zip  = 'https://github.com/Ed94/raylib_refactored/archive/refs/heads/refactor-support.zip'
	$path_raylib_zip = join-path $path_temp 'raylib.zip'

	$path_raylib_master      = join-path $path_temp          'raylib_refactored-refactor-support'
	$path_raylib_src         = join-path $path_raylib_master 'src'
	$path_raylib_platforms   = join-path $path_raylib_src    'platforms'
	$path_raylib_glfw_inc    = join-path $path_raylib_src    'external/glfw/include'
	$path_raylib_gputex      = join-path $path_raylib_src    'external/rl_gputex.h'

	if ( test-path $path_raylib_master ) {
		remove-item $path_raylib_master -Recurse
	}
	invoke-webrequest -uri $url_raylib_zip   -outfile $path_raylib_zip
	expand-archive    -path $path_raylib_zip -destinationpath $path_temp

	write-host "Building raylib with $vendor"

	$path_build = Join-Path $path_raylib 'build'
	if ( (Test-Path $path_build) -eq $false ) {
		New-Item $path_build -ItemType Directory
	}

	$raylib_headers  = Get-ChildItem -Path $path_raylib_src -Filter '*.h' -File
	$raylib_modules  = get-childitem -path $path_raylib_src -filter '*.c' -file

	# Refactor with refactor.exe
	if ( $true ) {
		$path_refactor      = join-path $path_raylib 'raylib_cpp.refactor'
		$path_refactor_rlgl = join-path $path_raylib 'raylib_cpp_gl.refactor'

		$files = @()
		foreach ( $header in $raylib_headers ) {
			$file_name = split-path $header -leaf

			if ( -not $file_name.Equals('rlgl.h' ) ) {
				$files += "$header"
			}
		}
		foreach ( $module in $raylib_modules ) {
			$files += "$module"
		}

		$files += "$path_raylib_gputex"

		$platform_modules = @()
		foreach ( $module in (get-childitem -path $path_raylib_platforms -filter '*.c' -file) ) {
			$platform_modules += "$module"
		}

		$path_rlgl = join-path $path_raylib_src 'rlgl.h'

		Push-Location $path_raylib_src
			write-host "Beginning refactor...`n"
			$refactors = @(@())
			$refactorParams = @(
				# "-debug",
				"-num=$($files.Count)"
				"-src=$($files)",
				"-spec=$($path_refactor)"
			)
			& refactor $refactorParams
			Write-Host "`nRefactoring complete`n`n"
		Pop-Location

		Push-Location $path_raylib_platforms
		write-host "Beginning refactor...`n"
			$refactors = @(@())
			$refactorParams = @(
				# "-debug",
				"-num=$($platform_modules.Count)"
				"-src=$($platform_modules)",
				"-spec=$($path_refactor)"
			)
			& refactor $refactorParams
			Write-Host "`nRefactoring complete`n`n"
		Pop-Location

		Push-Location $path_raylib_src
			$gl_modules = @( "$path_rlgl", "$path_raylib_gputex" )

			write-host "Beginning refactor just for rlgl.h...`n"
			$refactors = @(@())
			$refactorParams = @(
				# "-debug",
				"-num=$($gl_modules.Count)"
				"-src=$($gl_modules)",
				"-spec=$($path_refactor_rlgl)"
			)
			& refactor $refactorParams
			Write-Host "`nRefactoring complete`n`n"
		Pop-Location
	}

	# Refactor raylib with gencpp
	if ( $false ) {
	# if ( $false ) {
		$path_gencpp = join-path $path_root 'project/gen'

		$includes = @(
			$path_gencpp
		)

		$compiler_args = @(
			($flag_define + 'GEN_TIME')
		)

		$linker_args = @(

		)

		$unit       = join-path $path_raylib 'raylib_refactor.cpp'
		$executable = join-path $path_build  'raylib_refactor.exe'

		$build_result = build-simple $path_build $includes $compiler_args $linker_args $unit $executable
		Push-Location $path_raylib_src
			if ( Test-Path( $executable ) ) {
				Measure-Command { & $executable
						| ForEach-Object {
							write-host `t $_ -ForegroundColor Green
						}
					}
			}
		Pop-Location

		push-location $path_scripts
		# Time to format
		$fmt_includes = @()
		foreach ( $header in $raylib_headers ) {
			$fmt_includes +=  split-path $header -leaf
		}
		foreach ( $module in $raylib_modules ) {
			$fmt_includes +=  split-path $module -leaf
		}
		format-cpp $path_raylib_src $fmt_includes $null
		pop-location
	}

	# Build raylib
	if ( $true ) {
		# Microsoft
		$lib_gdi32   = 'Gdi32.lib'
		$lib_shell32 = 'Shell32.lib'
		$lib_xinput  = 'Xinput.lib'
		$lib_user32  = 'User32.lib'
		$lib_winmm   = 'Winmm.lib'

		$includes = @(
			$path_raylib_src,
			$path_raylib_glfw_inc
		)
		foreach ($include in $includes) {
			write-host $include
		}

		$compiler_args = @(
			($flag_define + 'PLATFORM_DESKTOP'),
			($flag_define + 'RL_BUILD_LIBTYPE_SHARED'),
			$flag_all_cpp
		)
		$linker_args   = @(
			$flag_link_dll,

			# $lib_xinput,
			$lib_gdi32,
			$lib_shell32,
			$lib_user32,
			$lib_winmm
		)

		# $unit = join-path $path_raylib     'raylib.c'
		$dll  = join-path $path_raylib_lib 'raylib.dll'
		# $build_result = build-simple $path_build $includes $compiler_args $linker_args $unit $dll

		$build_result = build $path_build $includes $compiler_args $linker_args $raylib_modules $dll
	}

	# Move headers to used include
	foreach ($header in $raylib_headers) {
		Copy-Item -Path $header -Destination (join-path $path_raylib_inc (split-path $header -Leaf))
	}
	remove-item -path $path_temp -Recurse
}
setup-raylib
