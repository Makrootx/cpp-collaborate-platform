param(
    [string]$CoreUrl = "https://www.codesynthesis.com/download/odb/2.5.0/windows/windows10/x86_64/libodb-2.5.0-x86_64-windows10-msvc17.10.zip",
    [string]$PgUrl   = "https://www.codesynthesis.com/download/odb/2.5.0/windows/windows10/x86_64/libodb-pgsql-2.5.0-x86_64-windows10-msvc17.10.zip",
    [string]$BoostUrl = "https://www.codesynthesis.com/download/odb/2.5.0/windows/windows10/x86_64/libodb-boost-2.5.0-x86_64-windows10-msvc17.10.zip",
    [string]$CompilerUrl = "https://www.codesynthesis.com/download/odb/2.5.0/windows/windows10/x86_64/odb-2.5.0-x86_64-windows10.zip",
    [string]$CompilerLinkPath = "C:\odb-2.5.0"
)

$ErrorActionPreference = 'Stop'

$Root = Split-Path -Parent $MyInvocation.MyCommand.Definition | Split-Path -Parent
$Deps = Join-Path $Root '.deps/odb-2.5'
New-Item -ItemType Directory -Force -Path $Deps | Out-Null
Set-Location $Deps

function Fetch-UnpackZip {
    param([string]$Url)
    $name = Split-Path $Url -Leaf
    if (-not (Test-Path $name)) {
        Invoke-WebRequest -Uri $Url -OutFile $name
    }
    $dirName = [System.IO.Path]::GetFileNameWithoutExtension($name)
    $targetDir = Join-Path $Deps $dirName
    if (-not (Test-Path $targetDir)) {
        $tmpDir = Join-Path $Deps "__tmp_$dirName"
        Expand-Archive -Path $name -DestinationPath $tmpDir -Force
        $children = Get-ChildItem $tmpDir
        if ($children.Count -eq 1 -and $children[0].PSIsContainer) {
            Move-Item $children[0].FullName $targetDir
            Remove-Item $tmpDir -Recurse -Force
        } else {
            Rename-Item $tmpDir $dirName
        }
    }
    return $targetDir
}

$CoreDir = Fetch-UnpackZip -Url $CoreUrl
$PgDir   = Fetch-UnpackZip -Url $PgUrl
$BoostDir = Fetch-UnpackZip -Url $BoostUrl
$CompilerDir = Fetch-UnpackZip -Url $CompilerUrl

$CompilerRoot = $CompilerDir
try {
    if (Test-Path $CompilerLinkPath) {
        $existing = Get-Item $CompilerLinkPath -ErrorAction Stop
        if ($existing.LinkType -eq 'Junction') {
            Remove-Item $CompilerLinkPath -Force
        }
    }
    if (-not (Test-Path $CompilerLinkPath)) {
        New-Item -ItemType Junction -Path $CompilerLinkPath -Target $CompilerDir | Out-Null
    }
    $CompilerRoot = $CompilerLinkPath
} catch {
    Write-Warning "Could not create compiler junction at $CompilerLinkPath. Using extracted path instead."
}

$CompilerExe = Join-Path $CompilerRoot 'bin/odb.exe'
if (-not (Test-Path $CompilerExe)) {
    $CompilerExe = Join-Path $CompilerRoot 'bin/odb-2.5.exe'
}

$envFile = Join-Path $Deps 'odb_env.ps1'
$envLines = @(
    "`$env:ODB_ROOT='$CoreDir'",
    "`$env:ODB_PGSQL_ROOT='$PgDir'",
    "`$env:ODB_BOOST_ROOT='$BoostDir'",
    "`$env:ODB_ROOT_MAIN='${CompilerRoot}/bin'"
)
if (Test-Path $CompilerExe) {
    $envLines += "`$env:ODB_COMPILER='$CompilerExe'"
}
$envLines | Out-File $envFile -Encoding ascii

Write-Host "ODB_ROOT=$CoreDir"
Write-Host "ODB_PGSQL_ROOT=$PgDir"
Write-Host "ODB_BOOST_ROOT=$BoostDir"
Write-Host "ODB_ROOT_MAIN=$CompilerRoot/bin"
if (Test-Path $CompilerExe) {
    Write-Host "ODB_COMPILER=$CompilerExe"
} else {
    Write-Warning "ODB compiler executable not found under $CompilerRoot/bin"
}
Write-Host "Wrote $envFile"
