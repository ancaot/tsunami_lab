Submission 1: Riemann Solver
============================
F-Wave Solver code
------------------

This is the first submission, in which a f-wave solver was implemented and tested. 

The given Roe solver was simply adjusted by P. Wächter to the different computation of the jump with the flux function.
The computation of the wave speed is the same in both the Roe and the f-wave solver, so no changes made there.

This meant a change was made to the function computing the wave strengths. 
The eigenvector matrix was now multiplied with the difference of following flux function for the right side and left side:
:math:`f := [hu, hu^2 + \frac{1}{2}gh^2]^T`.

The other change to the Roe solver is the computation of the scaled waves.
In difference to the Roe solver, the f-wave solver only uses the wave strength to scale the waves and not both wave speed and strength.
Thus the values of the output net-updates also changed.

Test Cases for the f-wave solver
--------------------------------

The test cases for the new f-wave solver are also based on the given Roe solver test cases, 
most of the test cases were adjusted by A. Otto with the exception of one, which was adjusted by P. Wächter.
Most computations made in the comments of the test cases were made with 'Wolfram Alpha <https://www.wolframalpha.com/>'.

The test case for the wave speed is identical to the Roe solver on basis of using the same computation process.

The test case for the wave strengths was adjusted to include the flux function as the f-wave solver uses it. 
As such the values for the wave strengths differ to the values in test cases of the Roe solver.

The test cases for the net-updates were also adjusted according to the f-wave solver.
The values computed differ only slightly to the values of the Roe solver test cases.

A test case for net-updates unique to the f-wave solver for a supersonic problem was added.
In this case the computed eigenvectors for both sides are positive (negative eigenvectors are analog to this case).
This case was added according to how the net-updates are influenced by the direction of the waves, numerical the value of the eigenvectors.
As such, this test confirms that one net-update stays at zero, as implied in the equations computing the net-updates.
