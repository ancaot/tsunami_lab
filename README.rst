###########
Tsunami Lab
###########
#Forked
This is the initial code of the Tsunami Lab taught at Friedrich Schiller University Jena.
Further information is available from: https://scalable.uni-jena.de/opt/tsunami/

CUDA prototype and final report
-------------------------------

The CUDA work started as standalone prototypes in ``cuda/``. In addition, the
main application can now select a minimal CUDA-backed ``WavePropagation2d``
implementation when it is built with ``TSUNAMI_LAB_ENABLE_CUDA``.

Relevant files:

* ``cuda/smoke_test.cu``: minimal CUDA compiler/runtime check.
* ``cuda/fwave_benchmark.cu``: isolated F-Wave CPU/OpenMP/CUDA benchmark.
* ``cuda/wavepropagation2d_benchmark.cu``: resident CUDA prototype of the
  numerical ``WavePropagation2d`` time step.
* ``src/patches/wavepropagation2d/WavePropagation2dCuda.cu``: CUDA-backed
  ``WavePropagation`` implementation used by the main application.
* ``docs/cuda_plan.md`` and ``docs/cuda_final_benchmark.md``: CUDA setup,
  benchmark results, limitations and interpretation.
* ``sphinx/source/chapters/submission_10.rst``: final written submission.

On Windows with CUDA Toolkit and Visual Studio Build Tools installed:

.. code-block:: powershell

   .\tools\build_cuda_smoke.ps1
   .\tools\build_cuda_fwave.ps1 -Edges 100000 -Iterations 50
   .\tools\build_cuda_wavepropagation2d.ps1 -GridSize 512 -Iterations 20
   .\tools\build_cuda_app.ps1

On a Linux/HPC system with CUDA modules:

.. code-block:: bash

   sbatch tools/cuda_fwave_benchmarks.sh
   sbatch tools/cuda_wavepropagation_benchmarks.sh

To use the CUDA backend in the main application, set
``"compute_backend": "cuda"`` in ``configs/config.json`` and run the executable
created by ``tools/build_cuda_app.ps1``.

Current limitation: the CUDA backend is integrated as a minimal main-application
path for ``2d`` + ``fwave``. It is not yet part of the normal SCons build and is
not yet connected to the full NetCDF/Tohoku/Chile/checkpoint production path.


-----------------------------------------------------------------------------
Important Links:
User Documentation ("Projektbericht") : https://sphnix-tsnunami.web.app
Doxygen Documentation                 : https://doxygen-tsunami.web.app

------------------------------------------------------------------------------
