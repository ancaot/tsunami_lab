Submission 4: Two-Dimensional Solver
=================================================

Task Stations
----------------------

In this task block, I extended the simulation output so that time series can be written at freely definable measurement points, called stations. The goal was to move beyond storing only full field snapshots as CSV and to support clean, flexible point-based observation. For this purpose, I added a small standalone IO component that loads stations from an external configuration and writes their values at fixed time intervals.

The most important design decision was to make station output independent of the number of time steps. Instead, output is controlled by simulation time. This keeps the output stable even if the time step size changes due to the CFL condition or different model parameters.

What I implemented
------------------

I introduced the new class ``tsunami_lab::io::Stations``. This class manages multiple stations with names and positions, loads them from CSV or XML files, and creates one output file per station. Each output file contains two columns, ``time`` and ``height``.

In addition, I extended the existing program entry point so that a station configuration file can be provided optionally. If a file is given, stations are loaded, output intervals are configured, and time series are written automatically during the simulation.

For runtime configuration, I integrated XML support using ``pugixml``. This is important because it makes the configuration easier to extend later. The XML format can define not only stations but also metadata such as output interval.

Key technical changes
---------------------

The main changes were:

* new files for station logic in ``src/io/Stations.h`` and ``src/io/Stations.cpp``
* extension of the main program in ``src/main.cpp`` for optional station output
* build system updates in ``SConstruct`` and ``src/SConscript``
* switch to C++17 to use modern library features, including ``std::filesystem``
* XML integration via ``pugixml``

I also added example configurations so usage can be reproduced directly:

* ``data/stations_example.csv``
* ``data/stations_example.xml``
* ``data/stations_symmetric.xml``

Why this matters
----------------

Stations provide a solid basis for later validation and comparison tasks. They allow measurements at fixed positions over time, which is more useful than only global snapshots when comparing numerical methods, symmetry behavior, or different model variants.

For later analysis, I also added a small plotting script to visualize generated CSV files directly. A separate comparison script is available as well to compare station outputs from two simulation runs.

What I intentionally did not implement
--------------------------------------

I did not implement the optional convergence-study part because it was explicitly marked as optional in this task block. I also did not add NetCDF input, since based on your note this belongs to a later assignment.

I also did not include images or GIFs in this chapter, because this task is mainly technical and structural and can be documented clearly with text and code references. If needed for submission, screenshots from ParaView or example plots can be added later.

Short conclusion
----------------

This task block extends the simulation with flexible station-based output. The configuration is now extensible, and output is time-controlled rather than step-controlled, making it more robust. This establishes a good foundation for later analysis and comparison workflows.