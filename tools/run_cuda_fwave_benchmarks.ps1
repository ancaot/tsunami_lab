param(
  [int[]] $GridSizes = @(128, 256, 512, 1024, 2048),
  [int] $Iterations = 100,
  [string] $CudaArch = "sm_89"
)

$ErrorActionPreference = "Stop"
$repoRoot = Resolve-Path (Join-Path $PSScriptRoot "..")
$buildScript = Join-Path $PSScriptRoot "build_cuda_fwave.ps1"
$executable = Join-Path $repoRoot "build\cuda\fwave_benchmark.exe"
$outputDirectory = Join-Path $repoRoot "outputs\cuda"
$csvPath = Join-Path $outputDirectory "fwave_final_benchmark.csv"

& $buildScript -CudaArch $CudaArch -BuildOnly
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
New-Item -ItemType Directory -Force -Path $outputDirectory | Out-Null

$results = foreach ($gridSize in $GridSizes) {
  # A square N x N grid has N*(N+1) horizontal and N*(N+1) vertical edges.
  [UInt64] $edges = 2 * [UInt64]$gridSize * ([UInt64]$gridSize + 1)
  Write-Host "`nBenchmarking ${gridSize}x${gridSize} grid ($edges edges)..."
  $output = & $executable $edges $Iterations 2>&1
  $output | ForEach-Object { Write-Host $_ }
  if ($LASTEXITCODE -ne 0) { throw "Benchmark failed for grid size $gridSize." }

  $csvLine = $output | Where-Object { $_ -like "CSV,*" } | Select-Object -Last 1
  if (-not $csvLine) { throw "CSV result missing for grid size $gridSize." }
  $values = $csvLine -split ','
  [PSCustomObject]@{
    grid_size = $gridSize
    edges = [UInt64]$values[1]
    serial_ms = [double]::Parse($values[2], [Globalization.CultureInfo]::InvariantCulture)
    openmp_ms = [double]::Parse($values[3], [Globalization.CultureInfo]::InvariantCulture)
    h2d_ms = [double]::Parse($values[4], [Globalization.CultureInfo]::InvariantCulture)
    cuda_kernel_ms = [double]::Parse($values[5], [Globalization.CultureInfo]::InvariantCulture)
    d2h_ms = [double]::Parse($values[6], [Globalization.CultureInfo]::InvariantCulture)
    cuda_e2e_ms = [double]::Parse($values[7], [Globalization.CultureInfo]::InvariantCulture)
    openmp_speedup = [double]::Parse($values[8], [Globalization.CultureInfo]::InvariantCulture)
    cuda_kernel_speedup = [double]::Parse($values[9], [Globalization.CultureInfo]::InvariantCulture)
    cuda_e2e_speedup = [double]::Parse($values[10], [Globalization.CultureInfo]::InvariantCulture)
    cuda_vs_openmp = [double]::Parse($values[11], [Globalization.CultureInfo]::InvariantCulture)
    cuda_mismatches = [UInt64]$values[12]
    openmp_mismatches = [UInt64]$values[13]
  }
}

$results | Export-Csv -Path $csvPath -NoTypeInformation -Encoding utf8
$results | Format-Table -AutoSize
Write-Host "Results written to $csvPath"
