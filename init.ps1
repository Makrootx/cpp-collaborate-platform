$ProjectRoot = Split-Path -Parent $MyInvocation.MyCommand.Definition

if (-not $env:VCPKG_ROOT) {
	$env:VCPKG_ROOT = "C:\vcpkg"
}

$localOdbEnv = Join-Path $ProjectRoot ".deps/odb-2.5/odb_env.ps1"
if (Test-Path $localOdbEnv) {
	. $localOdbEnv
}

if (-not $env:ODB_ROOT) {
	$env:ODB_ROOT = "C:\odb-2.5.0-msvc\libodb-2.5.0-x86_64-windows10-msvc17.10"
}
if (-not $env:ODB_PGSQL_ROOT) {
	$env:ODB_PGSQL_ROOT = "C:\odb-2.5.0-msvc\libodb-pgsql-2.5.0-x86_64-windows10-msvc17.10"
}
if (-not $env:ODB_BOOST_ROOT) {
	$env:ODB_BOOST_ROOT = "C:\odb-2.5.0-msvc\libodb-boost-2.5.0-x86_64-windows10-msvc17.10"
}
if (-not $env:ODB_ROOT_MAIN) {
	$env:ODB_ROOT_MAIN = "C:\odb-2.5.0\bin"
}

$pathParts = New-Object System.Collections.Generic.List[string]

if (Test-Path $env:VCPKG_ROOT) {
	$pathParts.Add($env:VCPKG_ROOT)
}

if ($env:ODB_ROOT -and (Test-Path (Join-Path $env:ODB_ROOT 'bin'))) {
	$pathParts.Add((Join-Path $env:ODB_ROOT 'bin'))
}

if ($env:ODB_ROOT_MAIN -and (Test-Path $env:ODB_ROOT_MAIN)) {
	$pathParts.Add($env:ODB_ROOT_MAIN)
}

if ($env:ODB_PGSQL_ROOT -and (Test-Path (Join-Path $env:ODB_PGSQL_ROOT 'bin'))) {
	$pathParts.Add((Join-Path $env:ODB_PGSQL_ROOT 'bin'))
}

if ($env:ODB_BOOST_ROOT -and (Test-Path (Join-Path $env:ODB_BOOST_ROOT 'bin'))) {
	$pathParts.Add((Join-Path $env:ODB_BOOST_ROOT 'bin'))
}

$pathParts.Add($env:PATH)
$env:PATH = ($pathParts -join ';')

$compilerCandidates = @(
	(Join-Path $env:ODB_ROOT_MAIN 'odb.exe'),
	(Join-Path $env:ODB_ROOT_MAIN 'odb-2.5.exe'),
	(Join-Path $ProjectRoot '.deps/odb-2.5/bin/odb.exe'),
	(Join-Path $ProjectRoot '.deps/odb-2.5/bin/odb-2.5.exe')
)

foreach ($candidate in $compilerCandidates) {
	if ($candidate -and (Test-Path $candidate)) {
		$env:ODB_COMPILER = $candidate
		break
	}
}

Write-Host "VCPKG_ROOT=$env:VCPKG_ROOT"
Write-Host "ODB_ROOT=$env:ODB_ROOT"
Write-Host "ODB_PGSQL_ROOT=$env:ODB_PGSQL_ROOT"
Write-Host "ODB_BOOST_ROOT=$env:ODB_BOOST_ROOT"
if ($env:ODB_COMPILER) {
	Write-Host "ODB_COMPILER=$env:ODB_COMPILER"
} else {
	Write-Warning "ODB compiler executable not found. Install ODB compiler or set ODB_COMPILER env var."
}