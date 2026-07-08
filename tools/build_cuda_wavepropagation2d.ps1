param(
  [int] $GridSize = 512,
  [int] $Iterations = 20,
  [string] $CudaArch = "sm_89",
  [string] $CudaPath = $env:CUDA_PATH
)

$ErrorActionPreference = "Stop"
if (-not $CudaPath) {
  $CudaPath = Get-ChildItem "C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA" -Directory |
    Sort-Object Name -Descending | Select-Object -First 1 -ExpandProperty FullName
}
$nvcc = Join-Path $CudaPath "bin\nvcc.exe"
$vsWhere = "C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe"
$vsRoot = & $vsWhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
$vsDevCmd = Join-Path $vsRoot "Common7\Tools\VsDevCmd.bat"
$root = Resolve-Path (Join-Path $PSScriptRoot "..")
$outDir = Join-Path $root "build\cuda"
$out = Join-Path $outDir "wavepropagation2d_benchmark.exe"
New-Item -ItemType Directory -Force -Path $outDir | Out-Null
$sources = @(
  (Join-Path $root "cuda\wavepropagation2d_benchmark.cu"),
  (Join-Path $root "src\patches\wavepropagation2d\WavePropagation2d.cpp"),
  (Join-Path $root "src\solvers\FWave.cpp"),
  (Join-Path $root "src\solvers\Roe.cpp")
)
$quotedSources = ($sources | ForEach-Object { "`"$_`"" }) -join " "
$command = "`"$vsDevCmd`" -arch=x64 && `"$nvcc`" -std=c++17 -O2 -arch=$CudaArch -Xcompiler=/openmp $quotedSources -o `"$out`""
cmd.exe /c $command
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
& $out $GridSize $Iterations
exit $LASTEXITCODE
