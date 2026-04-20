Submission 2: Finite Volume Discretization
==========================================

Tasks regarding the one-dimensional implementation of wave propagation
----------------------------------------------------------------------

The task was to include the f-wave solver in the given files implementing wave propagation, including the test file.
Additionally we should find a solution to switch between Roe and f-wave solver.
The switch was implemented with MODE macros and switch functions in the respective files.
The modes to choose from are MODEFWAVE and MODEROE, which then follow the chosen solver computations and tests. 

Tasks regarding the Shock and Rarefaction Waves
-----------------------------------------------

The Setup for the shock-shock problem...

**Observations for different sets:**

    #. A larger initial water height :math:`h_l` leads to a stronger shock and a bigger jump in the solution.
    #. Increasing the magnitude of the initial velocity :math:`u_l` makes the shock formation more pronounced when the streams move toward each other.
    #. This behavior is consistent with the wave speeds :math:`\lambda_{1/2} = u \mp \sqrt{g h}`.

The Setup for the rare-rare problem...

**Observations for different sets:**

    #. If the initial velocities point away from each other, the waves spread out and a rarefaction pattern appears.
    #. The solution becomes smoother and the water height changes more gradually than in the shock-shock case.
    #. The observed behavior again matches the signs of the wave speeds :math:`\lambda_{1/2} = u \mp \sqrt{g h}`.

Tasks regarding the Dam-Break
-----------------------------

The Setup for the dam-break problem...

**Observations for different sets:**

    #. Set - heights :math:`h_l=` and :math:`h_r=`

**Answer to Tasks 2 of 2.2:**

:math:`q_l=[14,0]^T` and :math:`q_r=[3.5,0.7]^T`

