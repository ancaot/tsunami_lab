.. tsunami_lab_otto_waechter documentation master file, created by
   sphinx-quickstart on Wed Apr 15 15:08:20 2026.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Welcome to Otto and Wächter's Tsunami Lab documentation!
========================================================

.. toctree::
   :maxdepth: 2
   :caption: Contents:


Submission 1: Riemann Solver
============================

This is the first submission, in which a f-wave solver was implemented and tested.

The implementation was straight forward, the given Roe solver was simply adjusted to the different computation of the jump with the flux function.

Most test cases were also based on the given test cases of the Roe solver, just changed to follow the computation directives of the f-wave solver. 
A test case for net updates for a supersonic problem was added, the other case is analog to the one we have added.


Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`
