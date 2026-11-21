<#
.SYNOPSIS
    Installs everything needed to build Contacts on Windows.

.DESCRIPTION
    - CMake, Ninja, Git (for Git Bash), GNU Make and Python via winget
    - Qt 6 for MinGW plus the matching MinGW compiler via aqtinstall
    - Adds Qt and MinGW to your user PATH and sets CMAKE_PREFIX_PATH

    Run from PowerShell:
        powershell -ExecutionPolicy Bypass -File scripts\install-deps-windows.ps1

    Then open a NEW "Git Bash" window in the project folder and run:  make run

.PARAMETER QtVersion
    Qt version to install (default 6.5.3, an LTS release).

.PARAMETER QtRoot
    Where Qt is installed (default C:\Qt).
#>
param(
    [string]$QtVersion = "6.5.3",
    [string]$QtRoot = "C:\Qt"
)

$ErrorActionPreference = "Stop"

function Info($message) { Write-Host "==> $message" -ForegroundColor Cyan }
function Warn($message) { Write-Host "warning: $message" -ForegroundColor Yellow }

function Refresh-Path {
    $env:Path = [Environment]::GetEnvironmentVariable("Path", "Machine") + ";" +
                [Environment]::GetEnvironmentVariable("Path", "User")
}

function Add-UserPath([string]$directory) {
    $current = [Environment]::GetEnvironmentVariable("Path", "User")
    $entries = @()
    if ($current) { $entries = $current.Split(";") | Where-Object { $_ -ne "" } }
    if ($entries -notcontains $directory) {
        [Environment]::SetEnvironmentVariable("Path", (($entries + $directory) -join ";"), "User")
        Info "Added to user PATH: $directory"
    }
}

if (-not (Get-Command winget -ErrorAction SilentlyContinue)) {
    throw "winget was not found. Install 'App Installer' from the Microsoft Store, then run this script again."
}

# --- 1. Build tools -----------------------------------------------------------
$packages = @(
    "Kitware.CMake",
    "Ninja-build.Ninja",
    "Git.Git",
    "ezwinports.make",
    "Python.Python.3.12"
)
foreach ($id in $packages) {
    Info "Installing $id"
    winget install --id $id --exact --silent --accept-source-agreements --accept-package-agreements
    if ($LASTEXITCODE -ne 0) { Warn "${id}: winget exit code $LASTEXITCODE (usually means it is already installed)" }
}
Refresh-Path

# --- 2. Qt + MinGW via aqtinstall ---------------------------------------------
$python = (Get-Command py -ErrorAction SilentlyContinue)
if (-not $python) { $python = (Get-Command python -ErrorAction SilentlyContinue) }
if (-not $python) { throw "Python was not found on PATH. Open a new PowerShell window and run this script again." }
$py = $python.Source

Info "Installing aqtinstall (Qt's unofficial command-line installer)"
& $py -m pip install --upgrade --user aqtinstall
if ($LASTEXITCODE -ne 0) { throw "pip install aqtinstall failed" }

Info "Installing Qt $QtVersion (MinGW 64-bit) into $QtRoot"
& $py -m aqt install-qt windows desktop $QtVersion win64_mingw --outputdir $QtRoot
if ($LASTEXITCODE -ne 0) { throw "Qt installation failed" }

Info "Installing the MinGW compiler that matches Qt $QtVersion"
& $py -m aqt install-tool windows desktop tools_mingw90 qt.tools.win64_mingw900 --outputdir $QtRoot
if ($LASTEXITCODE -ne 0) { throw "MinGW installation failed" }

# --- 3. Environment -----------------------------------------------------------
$qtDir = Join-Path $QtRoot "$QtVersion\mingw_64"
$mingwDir = Get-ChildItem (Join-Path $QtRoot "Tools") -Directory -Filter "mingw*_64" |
            Sort-Object Name -Descending | Select-Object -First 1
if (-not (Test-Path $qtDir)) { throw "Expected Qt at $qtDir but it is not there" }
if (-not $mingwDir) { throw "MinGW was not found under $QtRoot\Tools" }

Add-UserPath (Join-Path $qtDir "bin")
Add-UserPath (Join-Path $mingwDir.FullName "bin")
[Environment]::SetEnvironmentVariable("CMAKE_PREFIX_PATH", $qtDir, "User")
Info "Set CMAKE_PREFIX_PATH=$qtDir"

Refresh-Path
Info "Installed versions:"
cmake --version | Select-Object -First 1
ninja --version
& (Join-Path $mingwDir.FullName "bin\g++.exe") --version | Select-Object -First 1

Write-Host ""
Info "All set. Open a NEW Git Bash window in the project folder and run:  make run"
