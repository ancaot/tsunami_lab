###########
Tsunami Lab
###########

This is the initial code of the Tsunami Lab taught at Friedrich Schiller University Jena.
Further information is available from: https://scalable.uni-jena.de/opt/tsunami/

netCDF output
=============

The solver supports writing all selected snapshots to a single netCDF file
named solution.nc (COARDS-style coordinate and time metadata).

Build options:

- scons netcdf=auto (default): enable netCDF when libraries are found.
- scons netcdf=on: require netCDF and fail if not found.
- scons netcdf=off: disable netCDF output.

Optional paths for custom installations:

- scons netcdf=on netcdf_include=/path/to/include netcdf_libdir=/path/to/lib

Dependencies:

- netCDF (libnetcdf + netcdf.h)
- zlib and HDF5 (required by many netCDF builds, especially static linking)

Notes:

- The NetCdf writer is intended to exclude ghost cells by default.
- If your arrays already contain only physical cells, pass ghost offsets (0, 0)
	when calling writeTimeStep.

