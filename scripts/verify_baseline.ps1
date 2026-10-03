param(
    [Parameter(Mandatory = $true)][Alias('MirrorRoot')][string]$path_mirror,
    [ValidateSet(
		"measure",
		"positive",
		"negative-compile",
		"negative-link",
		"negative-launch",
		"negative-stale",
		"negative-stale-header",
		"negative-format",
		"negative-refactor",
		"negative-assert"
	)]
    [string]$mode = "positive",
    [Alias('SourceRoot')][string]$path_source_root = "C:\projects\gencpp",
    [Alias('ReuseMirror')][switch]$reuse_mirror
)

$ErrorActionPreference = "Stop"
$path_source           = (Resolve-Path -LiteralPath $path_source_root).Path
$path_mirror           = [System.IO.Path]::GetFullPath($path_mirror)
$path_verification_root = Join-Path $path_mirror 'test\verification'
if ($path_mirror -eq $path_source) { throw "MirrorRoot must not be the project checkout" }
$prefix_source = $path_source.TrimEnd('\') + '\'
$prefix_mirror = $path_mirror.TrimEnd('\') + '\'
if ($path_source.StartsWith($prefix_mirror, [System.StringComparison]::OrdinalIgnoreCase) -or
    $path_mirror.StartsWith($prefix_source, [System.StringComparison]::OrdinalIgnoreCase)) {
    throw "path_mirror and path_source_root must not overlap"
}
if ($reuse_mirror -and $mode -ne "positive") { throw "reuse_mirror is for the positive mode only" }
if ($reuse_mirror -and -not (Test-Path -LiteralPath (Join-Path $path_mirror 'base\base.cpp'))) {
    throw "reuse_mirror needs an existing baseline mirror"
}

function Copy-Snapshot([string]$path_destination) 
{
    if (Test-Path -LiteralPath $path_destination) { Remove-Item -LiteralPath $path_destination -Recurse -Force }
    New-Item -ItemType Directory -Path $path_destination | Out-Null
    & robocopy $path_source $path_destination /E /NFL /NDL /NJH /NJS /XD .git
    if ($LASTEXITCODE -ge 8) { throw "robocopy failed: $LASTEXITCODE" }
    $global:LASTEXITCODE = 0
    @(
        "gen_segmented\gen",
		"gen_segmented\build",
        "gen_singleheader\gen",
		"gen_singleheader\build",
        "gen_c_library\gen",
		"gen_c_library\build",
        "gen_unreal_engine\gen",
		"gen_unreal_engine\build",
        "test\c_library\build",
		"test\c_library\gen",
        "test\cpp_library\build",
		"test\cpp_library\gen",
        "base\build"
    ) | ForEach-Object {
        $path_item = Join-Path $path_destination $_
        if (Test-Path -LiteralPath $path_item) { Remove-Item -LiteralPath $path_item -Recurse -Force }
    }
}

function Copy-VerificationFixture([string]$name_fixture, [string]$path_destination) {
    $path_fixture = Join-Path $path_verification_root $name_fixture
    if (-not (Test-Path -LiteralPath $path_fixture -PathType Leaf)) {
        throw "Missing verification fixture: $path_fixture"
    }
    Copy-Item -LiteralPath $path_fixture -Destination $path_destination -Force
}

function Invoke-Driver([string]$path_tree, [string[]]$list_targets) {
    $path_driver = Join-Path $path_tree "scripts\build.ci.ps1"
    $name_log = if ($reuse_mirror) { 'verify_driver.repeat.log' } else { 'verify_driver.log' }
    $path_log = Join-Path $path_tree $name_log
    $path_previous_root = $env:GENCPP_SOURCE_ROOT
    $env:GENCPP_SOURCE_ROOT = $path_tree
    $line_args = $list_targets -join ' '
    try {
        $text_output = & powershell.exe -NoProfile -Command "& '$path_driver' clang debug $line_args; exit `$LASTEXITCODE" *>&1
        $code_exit = $LASTEXITCODE
    }
    finally {
        $env:GENCPP_SOURCE_ROOT = $path_previous_root
    }
    $text_output | Tee-Object -FilePath $path_log | Out-Host
    $script:path_last_driver_log  = $path_log
    $script:code_last_driver = $code_exit
    return $code_exit
}

function Get-DriverLogText {
    if (-not $script:path_last_driver_log) { return "" }
    if (-not (Test-Path -LiteralPath $script:path_last_driver_log)) { return "" }
    return Get-Content -LiteralPath $script:path_last_driver_log -Raw
}

function Assert-PositiveOutputs([string]$path_tree) {
    $list_outputs = @(
        'base\components\gen\ecodetypes.hpp', 
		'base\components\gen\eoperator.hpp',
        'base\components\gen\especifier.hpp', 
		'base\components\gen\etoktype.hpp',
        'base\components\gen\ast_inlines.hpp',
        'gen_segmented\gen\gen.hpp',
		'gen_segmented\gen\gen.cpp',
        'gen_segmented\gen\gen.dep.hpp',
		'gen_segmented\gen\gen.dep.cpp',
        'gen_segmented\gen\gen.builder.hpp',
		'gen_segmented\gen\gen.builder.cpp',
        'gen_segmented\gen\gen.scanner.hpp',
		'gen_segmented\gen\gen.scanner.cpp',
        'gen_singleheader\gen\gen.hpp',
        'gen_c_library\gen\gen.h',
		'gen_c_library\gen\gen.c',
        'gen_c_library\gen\gen.dep.h',
		'gen_c_library\gen\gen.dep.c',
        'gen_c_library\gen\gen_singleheader.h'
    )
    $list_hashes = foreach ($path_relative in $list_outputs) {
        $path_output = Join-Path $path_tree $path_relative
        if (-not (Test-Path -LiteralPath $path_output -PathType Leaf)) { throw "Missing output: $path_output" }
        if ((Get-Item -LiteralPath $path_output).Length -eq 0) { throw "Empty output: $path_output" }
        "{0}  {1}" -f (Get-FileHash -LiteralPath $path_output -Algorithm SHA256).Hash, $path_relative
    }
    $text_log = (Get-DriverLogText).Replace('\\', '\')
    foreach ($path_relative in @('gen_singleheader\gen\gen.hpp', 'gen_c_library\gen\gen_singleheader.h')) {
        $path_header = Join-Path $path_tree $path_relative
        if ($text_log -notmatch [regex]::Escape($path_header)) { throw "Compiler trace missing $path_header" }
    }
    $name_hash_file = if ($reuse_mirror) { 'verify_outputs.repeat.sha256' } else { 'verify_outputs.sha256' }
    $list_hashes | Set-Content -LiteralPath (Join-Path $path_tree $name_hash_file)
    Write-Host "PASSED: outputs and consumer header traces ($name_hash_file)"
}

$list_targets = @("base", "segmented", "singleheader", "c_lib", "test")
if (-not $reuse_mirror) { Copy-Snapshot $path_mirror }

if ($mode -eq "measure" -or $mode -eq "positive") {
    $code_exit = Invoke-Driver $path_mirror $list_targets
    if ($mode -eq "positive" -and $code_exit -ne 0) { exit $code_exit }
    if ($mode -eq "positive") { Assert-PositiveOutputs $path_mirror }
    exit $code_exit
}

if ($mode -eq "negative-compile") {
    Copy-VerificationFixture 'failure_compile.cpp' (Join-Path $path_mirror 'base\base.cpp')
    $code_exit = Invoke-Driver $path_mirror @("base")
    if ($code_exit -eq 0) { throw "negative-compile returned 0" }
    exit 0
}

if ($mode -eq "negative-link") {
    Copy-VerificationFixture 'failure_link.cpp' (Join-Path $path_mirror 'base\base.cpp')
    $code_exit = Invoke-Driver $path_mirror @("base")
    $text_log = Get-DriverLogText
    if ($code_exit -eq 0 -or $text_log -notmatch "Linking failed") {
        throw "negative-link did not fail at the linker"
    }
    exit 0
}

if ($mode -eq "negative-launch") {
    $path_toolchain = Join-Path $path_mirror "scripts\helpers\vendor_toolchain.ps1"
    $text_toolchain = Get-Content -LiteralPath $path_toolchain -Raw
    $text_patched   = $text_toolchain.Replace(
		"`$linker = 'lld-link'",
		"`$linker = 'gencpp-missing-linker'"
	)
    if ($text_patched -eq $text_toolchain) { throw "negative-launch could not replace the linker path" }

    Set-Content -LiteralPath $path_toolchain -Value $text_patched -NoNewline
    $code_exit = Invoke-Driver $path_mirror @("base")
    $text_log  = Get-DriverLogText
    if ($code_exit -eq 0 -or $text_log -notmatch "FAILED: linker launch gencpp-missing-linker") {
        throw "negative-launch did not report a missing linker"
    }
    exit 0
}

if ($mode -eq "negative-stale") {
    $path_stale_source     = Join-Path $path_mirror "stale_marker.c"
    $path_stale_executable = Join-Path $path_mirror "stale_marker.exe"
    Copy-VerificationFixture 'stale_executable.c' $path_stale_source

    & clang -o $path_stale_executable $path_stale_source
    if ($LASTEXITCODE -ne 0) { throw "failed to compile stale marker" }

    $path_base_build = Join-Path $path_mirror "base\build"
    New-Item -ItemType Directory -Path $path_base_build -Force | Out-Null
    Copy-Item -LiteralPath $path_stale_executable -Destination (Join-Path $path_base_build "base.exe") -Force
    Copy-VerificationFixture 'failure_compile.cpp' (Join-Path $path_mirror 'base\base.cpp')

    $code_exit = Invoke-Driver $path_mirror @("base")
    if ($code_exit -eq 0)  { throw "negative-stale returned 0" }
    if ($code_exit -eq 73) { throw "negative-stale ran the stale executable" }
    exit 0
}

if ($mode -eq "negative-stale-header") {
    $code_exit = Invoke-Driver $path_mirror @("base", "segmented", "singleheader", "c_lib")
    if ($code_exit -ne 0) { throw "negative-stale-header setup generation failed: $code_exit" }

    Copy-Item -LiteralPath $script:path_last_driver_log -Destination (Join-Path $path_mirror 'verify_driver.setup.log')
    $path_header = Join-Path $path_mirror 'gen_c_library\gen\gen_singleheader.h'
    if (-not (Test-Path -LiteralPath $path_header)) { throw 'negative-stale-header missing setup header' }

    Copy-VerificationFixture 'failure_compile.cpp' (Join-Path $path_mirror 'gen_c_library\c_library.cpp')
    $code_exit = Invoke-Driver $path_mirror @('c_lib', 'test')
    $text_log  = Get-DriverLogText
    if ($code_exit -eq 0 -or $text_log -notmatch 'FAILED: c_lib compile/link') {
        throw 'negative-stale-header did not fail at c_lib'
    }
    if ($text_log -match 'Compiling .*test[.]c|PASSED: test-c11|PASSED: test-cpp') {
        throw 'negative-stale-header consumed a prior header'
    }
    exit 0
}

if ($mode -eq "negative-format") {
    $path_fake_tools = Join-Path $path_mirror "fake-tools"
    New-Item -ItemType Directory -Path $path_fake_tools -Force | Out-Null
    Copy-VerificationFixture 'failure_format.cmd' (Join-Path $path_fake_tools 'clang-format.cmd')
    $path_saved = $env:PATH
    $env:PATH   = "$path_fake_tools;$env:PATH"
    try {
        $code_exit = Invoke-Driver $path_mirror @("base")
    }
    finally {
        $env:PATH = $path_saved
    }
    $text_log = Get-DriverLogText
    if ($code_exit -eq 0)                          { throw "negative-format returned 0" }
    if ($text_log -notmatch "clang-format failed") { throw "negative-format log missing clang-format failed" }
    exit 0
}

if ($mode -eq "negative-refactor") {
    $path_fake_source     = Join-Path $path_mirror 'failing_refactor.c'
    $path_fake_executable = Join-Path $path_mirror 'scripts\helpers\refactor.exe'
    Copy-VerificationFixture 'failure_refactor.c' $path_fake_source
    & clang -o $path_fake_executable $path_fake_source
    if ($LASTEXITCODE -ne 0) { throw 'negative-refactor failed to compile the stub' }

    $code_exit = Invoke-Driver $path_mirror @('c_lib')
    $text_log  = Get-DriverLogText
    if ($code_exit -eq 0 -or $text_log -notmatch 'refactor failed') {
        throw 'negative-refactor did not propagate subprocess failure'
    }
    exit 0
}

if ($mode -eq "negative-assert") {
    $path_header = Join-Path $path_mirror "gen_singleheader\gen\gen.hpp"
    if (-not (Test-Path -LiteralPath $path_header)) {
        $code_exit = Invoke-Driver $path_mirror $list_targets
        if ($code_exit -ne 0) { throw "negative-assert setup generation failed: $code_exit" }
    }

    Copy-VerificationFixture 'failure_assert.cpp' (Join-Path $path_mirror 'test\cpp_library\test.cpp')
    $code_exit = Invoke-Driver $path_mirror @("test")
    $text_log  = Get-DriverLogText
    if ($code_exit -eq 0)                            { throw "negative-assert returned 0" }
    if ($text_log -notmatch "FAIL forced assertion") { throw "negative-assert log missing forced assertion marker" }
    exit 0
}

throw "unhandled mode $mode"
