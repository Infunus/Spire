param(
    [string]$BuildDir = "$PSScriptRoot\build-codex-verify\release",
    [string]$QtBin = "D:\Qt\6.10.2\mingw_64\bin",
    [string]$OutputDir = "$PSScriptRoot\dist\Spire"
)

$ErrorActionPreference = "Stop"

$projectRoot = [System.IO.Path]::GetFullPath($PSScriptRoot)
$outputPath = [System.IO.Path]::GetFullPath($OutputDir)
$buildPath = [System.IO.Path]::GetFullPath($BuildDir)
$sourceExe = Join-Path $buildPath "Spire.exe"
$deployTool = Join-Path $QtBin "windeployqt.exe"
$resourceSource = Join-Path $projectRoot "resources"
$outputExe = Join-Path $outputPath "Spire.exe"
$resourceOutput = Join-Path $outputPath "resources"

if (-not $outputPath.StartsWith($projectRoot, [System.StringComparison]::OrdinalIgnoreCase)) {
    throw "OutputDir must stay inside the project directory."
}
if (-not (Test-Path -LiteralPath $sourceExe -PathType Leaf)) {
    throw "Release executable not found: $sourceExe"
}
if (-not (Test-Path -LiteralPath $deployTool -PathType Leaf)) {
    throw "windeployqt not found: $deployTool"
}
if (-not (Test-Path -LiteralPath $resourceSource -PathType Container)) {
    throw "Resource directory not found: $resourceSource"
}

New-Item -ItemType Directory -Force -Path $outputPath | Out-Null
New-Item -ItemType Directory -Force -Path $resourceOutput | Out-Null

Copy-Item -LiteralPath $sourceExe -Destination $outputExe -Force
Copy-Item -Path (Join-Path $resourceSource "*") -Destination $resourceOutput -Recurse -Force

& $deployTool --release --compiler-runtime --no-translations $outputExe
if ($LASTEXITCODE -ne 0) {
    throw "windeployqt failed with exit code $LASTEXITCODE"
}

Write-Host "Release package created at: $outputPath"
