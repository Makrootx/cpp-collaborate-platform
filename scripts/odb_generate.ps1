param()

$ErrorActionPreference = 'Stop'

$ProjectRoot = Split-Path -Parent $PSScriptRoot
$SrcDir = Join-Path $ProjectRoot 'src'

function Resolve-OdbCompiler {
    $candidates = New-Object System.Collections.Generic.List[string]

    if ($env:ODB_COMPILER) {
        $candidates.Add($env:ODB_COMPILER)
    }

    $cmd = Get-Command odb -ErrorAction SilentlyContinue
    if ($cmd) {
        $candidates.Add($cmd.Source)
    }

    $cmdExe = Get-Command odb.exe -ErrorAction SilentlyContinue
    if ($cmdExe) {
        $candidates.Add($cmdExe.Source)
    }

    $cmdVer = Get-Command odb-2.5.exe -ErrorAction SilentlyContinue
    if ($cmdVer) {
        $candidates.Add($cmdVer.Source)
    }

    $candidates.Add((Join-Path $ProjectRoot '.deps/odb-2.5/bin/odb.exe'))
    $candidates.Add((Join-Path $ProjectRoot '.deps/odb-2.5/bin/odb-2.5.exe'))

    foreach ($candidate in $candidates) {
        if ($candidate -and (Test-Path $candidate)) {
            return $candidate
        }
    }

    throw "ODB compiler not found. Install ODB compiler and ensure 'odb.exe' is on PATH, or set ODB_COMPILER to full executable path."
}

$odbExe = Resolve-OdbCompiler

$includeArgs = @('-I', $SrcDir)
if ($env:ODB_ROOT) {
    $includeArgs += @('-I', (Join-Path $env:ODB_ROOT 'include'))
}
if ($env:ODB_PGSQL_ROOT) {
    $includeArgs += @('-I', (Join-Path $env:ODB_PGSQL_ROOT 'include'))
}

$vcpkgBuildInclude = Join-Path $ProjectRoot 'build_dev/vcpkg_installed/x64-windows/include'
if (Test-Path $vcpkgBuildInclude) {
    $includeArgs += @('-I', $vcpkgBuildInclude)
}

$vcpkgRootInclude = Join-Path $ProjectRoot 'vcpkg_installed/x64-windows/include'
if (Test-Path $vcpkgRootInclude) {
    $includeArgs += @('-I', $vcpkgRootInclude)
}

$entityHeaders = Get-ChildItem -Path $SrcDir -Recurse -File -Include '*.hpp', '*.h' |
    Where-Object {
        Select-String -Path $_.FullName -Pattern '^\s*#pragma\s+db\s+object\b' -Quiet
    }

if (-not $entityHeaders -or $entityHeaders.Count -eq 0) {
    Write-Host 'No ODB entity headers found (#pragma db object).'
    exit 0
}

foreach ($header in $entityHeaders) {
    $outDir = Split-Path -Parent $header.FullName

    $arguments = @(
        '-d', 'pgsql'
        '--std', 'c++20'
        '--generate-query'
        '--generate-schema'
        '--generate-session'
        $includeArgs
        '--output-dir', $outDir
        $header.FullName
    )

    Write-Host "Generating ODB files for $($header.FullName)"
    & $odbExe @arguments
    if ($LASTEXITCODE -ne 0) {
        throw "odb generation failed with exit code $LASTEXITCODE for $($header.FullName)"
    }
}

Write-Host 'ODB generation completed.'
