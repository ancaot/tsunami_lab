param(
  [string] $CudaPath = $env:CUDA_PATH,
  [string] $CudaArch = "sm_89",
  [UInt64] $Edges = 1000000,
  [int] $Iterations = 100
)

$ErrorActionPreference = "Stop"
if (-not $CudaPath) {
  $CudaPath = Get-ChildItem "C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA" -Directory -ErrorAction SilentlyContinue |
    Sort-Object Name -Descending |
    Where-Object { Test-Path (Join-Path $_.FullName "bin\nvcc.exe") } |
    Select-Object -First 1 -ExpandProperty FullName
}
if (-not $CudaPath) { throw "CUDA Toolkit not found. Pass -CudaPath explicitly." }

$nvcc = Join-Path $CudaPath "bin\nvcc.exe"
$vsWhere = "C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe"
$vsDevCmd = $null
if (Test-Path $vsWhere) {
  $vsRoot = & $vsWhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
  if ($vsRoot) {
    $candidate = Join-Path $vsRoot "Common7\Tools\VsDevCmd.bat"
    if (Test-Path $candidate) { $vsDevCmd = $candidate }
  }
}
if (-not $vsDevCmd) { throw "Visual Studio C++ Build Tools not found." }

$repoRoot = Resolve-Path (Join-Path $PSScriptRoot "..")
$source = Join-Path $repoRoot "cuda\fwave_benchmark.cu"
$cpuSource = Join-Path $repoRoot "src\solvers\FWave.cpp"
$outDir = Join-Path $repoRoot "build\cuda"
$outExe = Join-Path $outDir "fwave_benchmark.exe"
New-Item -ItemType Directory -Force -Path $outDir | Out-Null

$command = "`"$vsDevCmd`" -arch=x64 && `"$nvcc`" -std=c++17 -O2 -arch=$CudaArch `"$source`" `"$cpuSource`" -o `"$outExe`""
cmd.exe /c $command
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

& $outExe $Edges $Iterations
exit $LASTEXITCODE
