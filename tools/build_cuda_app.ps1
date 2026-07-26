param(
  [string] $CudaPath = $env:CUDA_PATH,
  [string] $CudaArch = "sm_89"
)

$root = Resolve-Path (Join-Path $PSScriptRoot "..")
if (-not $CudaPath) {
  $CudaPath = Get-ChildItem "C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA" -Directory -ErrorAction SilentlyContinue |
    Where-Object { Test-Path (Join-Path $_.FullName "bin\nvcc.exe") } |
    Sort-Object FullName -Descending |
    Select-Object -ExpandProperty FullName -First 1
}
if (-not $CudaPath) { throw "CUDA Toolkit not found. Pass -CudaPath explicitly." }

$nvcc = Join-Path $CudaPath "bin\nvcc.exe"
$vsWhere = "C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe"
$vsInstall = & $vsWhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if (-not $vsInstall) { throw "Visual Studio Build Tools with C++ workload not found." }
$vsDevCmd = Join-Path $vsInstall "Common7\Tools\VsDevCmd.bat"

$outDir = Join-Path $root "build\cuda"
New-Item -ItemType Directory -Force -Path $outDir | Out-Null
$out = Join-Path $outDir "tsunami_lab_cuda.exe"

$sources = @(
  "src\main.cpp",
  "src\patches\wavepropagation2d\WavePropagation2dCuda.cu",
  "src\patches\wavepropagation2d\WavePropagation2d.cpp",
  "src\patches\wavepropagation1d\WavePropagation1d.cpp",
  "src\solvers\FWave.cpp",
  "src\solvers\Roe.cpp",
  "src\setups\dambreak\DamBreak1d.cpp",
  "src\setups\dambreak2d\DamBreak2d.cpp",
  "src\setups\ArtificialTsunami2d\ArtificialTsunami2d.cpp",
  "src\setups\tsunamievent1d\TsunamiEvent1d.cpp",
  "src\setups\shockshock\ShockShock.cpp",
  "src\setups\rarerare\RareRare.cpp",
  "src\setups\subcriticalflow\SubcriticalFlow.cpp",
  "src\setups\supercriticalflow\SupercriticalFlow.cpp",
  "src\io\Csv\Csv.cpp",
  "src\io\Stations\Station.cpp",
  "src\io\JsReader\Configuration.cpp",
  "libs\pugixml-1.15\src\pugixml.cpp"
) | ForEach-Object { Join-Path $root $_ }

$allSources = $sources
$quotedSources = ($allSources | ForEach-Object { "`"$_`"" }) -join " "
$includeFlags = "-I`"$root\submodules\json\single_include`" -I`"$root\libs\pugixml-1.15\src`""

$command = "`"$vsDevCmd`" -arch=x64 && `"$nvcc`" -std=c++17 -O2 -arch=$CudaArch -DTSUNAMI_LAB_ENABLE_CUDA -Xcompiler=/openmp $includeFlags $quotedSources -o `"$out`""
cmd.exe /c $command
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

Write-Host "Built $out"
