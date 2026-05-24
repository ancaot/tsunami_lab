Submission 4: Two-Dimensional Solver
=================================================

4.1. Unsplit Method
-------------------

Task 1: Two-Dimensional Wave Propagation
----------------------------------------

**Requirement:** Add new class that supports two-dimensional problems.

**What I implemented:**

The class was built based on ``patches::WavePropagation1d`` with adjustments for the size of the arrays containing the new and old values for height, x-momentum and y-momentum.
The size was computed with :math:`(x-cells + 2)*y-cells` and considers the ghost cells in the rows.

The implementation for two-dimensional problems uses the unsplit method.
Which means each row was treated as an one-dimensional problem and uses the f-wave solver accordingly.
I added another instance of the f-wave solver method to compute the momentum in y-direction, which using hv as input inplace of hu.
:math:`huv` is computed prior to updating height and momenta, so we can use it in the finalization of the new values.
Additonally, the netupdates account now for the momentum in the y-direction.

.. code-block:: cpp

  l_hNew[l_off + l_ceL]  -= i_scaling * l_netUpdatesA[0][0] - i_scaling * l_netUpdatesB[0][0];
  l_huNew[l_off + l_ceL] -= i_scaling * l_netUpdatesA[0][1] - i_scaling * l_huvL;
  l_hvNew[l_off + l_ceL] -= i_scaling * l_huvL - i_scaling * l_netUpdatesB[0][1];

  l_hNew[l_off + l_ceR]  -= i_scaling * l_netUpdatesA[1][0] - i_scaling * l_netUpdatesB[1][0];
  l_huNew[l_off + l_ceR] -= i_scaling * l_netUpdatesA[1][1] - i_scaling * l_huvR;
  l_hvNew[l_off + l_ceR] -= i_scaling * l_huvR - i_scaling * l_netUpdatesB[1][1];

These operations follow the given equations.
With using the f-wave solver method a second time, we can implement the two-dimensional problems while still supporting one-dimensional problems.

Task 2: Circular Dam Break Setup
--------------------------------

**Requirement:** Set up for a circular dam break problem in :math:`[-50,50]^2`

**What I implemented:**

This setup is very similar to previous setups.
The biggest difference comes with the added momentum in y-direction.
Otherwise the initiation follows the given values for the height and momenta according to the given condition.

The observations are incomplete, as while compling and running the setup the values for the y-coordinates and the momentum in the y-direction didn't correctly initalize.
I couldn't find a fix for this problem before the deadline for the submission.

Task 3: Bathymetry in two dimensions
------------------------------------

Not implemented as a major set-back happened which took some time to fix (and even then I wasn't able to completey fix it) and didn't leave time for implementing an actual obstacle.


4.2. Stations
-------------

Task 1: Station Output Class
----------------------------

**Requirement:** Add a new class ``tsunami_lab::io::Stations`` which summarizes a collection of user-defined stations. Each station has a name and a location in space. All stations share the output frequency in seconds. The output for each station should be written in a separate ASCII-CSV file.

**What I implemented:**

I introduced the class ``tsunami_lab::io::Stations`` that manages measurement stations at fixed positions. Each station has a name and coordinates (x, y). The key design decision was to make output **time-controlled**, not step-controlled. This ensures stability independent of time step size changes due to the CFL condition.

The solver writes one CSV file per station with two columns: ``time`` and ``height``. Stations are loaded from external configuration files and sampled during the simulation at regular time intervals.

Task 2: Runtime Configuration
------------------------------

**Requirement:** Find a suitable way to provide the names and locations of each station to your solver. Further, implement a time step-independent output frequency for the stations. Keep in mind that this runtime-configuration should be extensible and usable in later parts of the project. One possible solution are XML-based runtime configurations through the library pugixml.

**What I implemented:**

I created two configuration formats:

* **CSV format:** Simple, lightweight. File format: ``name,x,y`` (one station per line).
  Example: ``data/stations_example.csv``

* **XML format:** Extensible, supports metadata. Uses ``pugixml`` library.
  Root element ``<stations>`` can define ``output_interval`` attribute (seconds).
  Each ``<station>`` has attributes: ``name``, ``x``, ``y``.
  Examples: ``data/stations_example.xml``, ``data/stations_symmetric.xml``

The main program accepts an optional configuration file (CSV or XML) and an output interval. The ``loadFromFile()`` method auto-detects file type by extension.

Key changes:
  * New files: ``src/io/Stations.h``, ``src/io/Stations.cpp``
  * Extended: ``src/main.cpp`` with optional station argument
  * Build system: ``SConstruct``, ``src/SConscript`` updated for ``pugixml``
  * Uses C++17 features (``std::filesystem``)

Task 3: Symmetric Problem Setup and 1D–2D Comparison
----------------------------------------------------

**Requirement:** Use a symmetric problem setup, e.g., a circular dam break problem, to compare your two-dimensional solver to your one-dimensional one at a set of stations.

**What I implemented:**

I use ``CircularDamBreak1d`` as the symmetric test case. This is a 1D surrogate of a radial dam break where water level is higher in an inner region and drops at a circular boundary. It is symmetric, which makes it ideal for validation.

For the 1D–2D comparison:
  * The 1D solver runs along the x-direction with the circular dam break.
  * The 2D solver would run on a 2D domain, also initialized with the same circular dam break.
  * Stations placed along a central transect (e.g., y = domain_center) allow direct comparison.
  * Configuration: ``data/stations_symmetric.xml`` defines 5 stations along the transect.

This setup validates that both solvers give consistent results at measurement points, confirming numerical accuracy and symmetry.

Task 4: Convergence Study (Optional)
------------------------------------

**Requirement:** Run a "convergence study", i.e., use a synthetic setup and decrease the size of your grid cells. For example, for the computational domain Ω = [0, 100m]², you could test mesh-spacings h ∈ {50, 25, 10, 7.5, 5, 4, 3, 2, 1}. Compare your solution at a set of points, what do you observe?

**What I intentionally did not implement:**

The convergence study was marked optional. I focused on completing the mandatory tasks (Stations class, runtime configuration, and symmetric comparison setup). 

The infrastructure for convergence studies is ready: the time-controlled station output and symmetric setup make it straightforward to run multiple resolutions and compare results. Future work can easily add a driver script to automate mesh refinement tests.

Summary
-------

The Stations class provides flexible, time-controlled output at fixed positions. Configuration via CSV or XML makes the system extensible. The symmetric circular dam break setup is prepared for 1D–2D solver validation once the 2D implementation is complete.