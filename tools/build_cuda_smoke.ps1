param(
  [string] $CudaPath = $env:CUDA_PATH,
  [string] $CudaArch = "sm_89"
)

$ErrorActionPreference = "Stop"

if (-not $CudaPath) {
  $defaultRoots = @(
    "C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v13.3",
    "C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v13.2",
    "C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v13.1",
    "C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v13.0",
    "C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v12.9",
    "C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v12.8",
    "C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v12.7",
    "C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v12.6"
  )

  $CudaPath = $defaultRoots | Where-Object { Test-Path (Join-Path $_ "bin\nvcc.exe") } | Select-Object -First 1
}

if (-not $CudaPath) {
  throw "CUDA Toolkit not found. Install it from https://developer.nvidia.com/cuda/toolkit or pass -CudaPath."
}

$nvcc = Join-Path $CudaPath "bin\nvcc.exe"
if (-not (Test-Path $nvcc)) {
  throw "nvcc.exe not found at $nvcc"
}

$vsDevCmd = "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\Common7\Tools\VsDevCmd.bat"
if (-not (Test-Path $vsDevCmd)) {
  $vsWhere = "C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe"
  if (Test-Path $vsWhere) {
    $vsRoot = & $vsWhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
    if ($vsRoot) {
      $candidate = Join-Path $vsRoot "Common7\Tools\VsDevCmd.bat"
      if (Test-Path $candidate) {
        $vsDevCmd = $candidate
      }
    }
  }
}

$repoRoot = Resolve-Path (Join-Path $PSScriptRoot "..")
$source = Join-Path $repoRoot "cuda\smoke_test.cu"
$outDir = Join-Path $repoRoot "build\cuda"
$outExe = Join-Path $outDir "smoke_test.exe"

New-Item -ItemType Directory -Force -Path $outDir | Out-Null

if (Test-Path $vsDevCmd) {
  $command = "`"$vsDevCmd`" -arch=x64 && `"$nvcc`" -std=c++17 -O2 -arch=$CudaArch `"$source`" -o `"$outExe`""
  cmd.exe /c $command
  if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
  }
}
else {
  & $nvcc -std=c++17 -O2 -arch=$CudaArch $source -o $outExe
  if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
  }
}

& $outExe
exit $LASTEXITCODE
