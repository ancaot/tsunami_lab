Submission 2: Finite Volume Discretization
==========================================

Tasks regarding the one-dimensional implementation of wave propagation
----------------------------------------------------------------------

The task was to include the f-wave solver in the given files implementing wave propagation, including the test file.
Additionally we should find a solution to switch between Roe and f-wave solver.
The switch was implemented with a macro MODE that switches functions based on its given value in the macros.h file.
The values that MODE can take are currently given by MODEFWAVE and MODEROE but more could be added to the file. 

Tasks regarding the Shock and Rarefaction Waves
-----------------------------------------------

The Setup for the shock-shock problem follows the formulas and conditions given in the task.
The setup is given the height, velocity and location discontinuty as input.
This means the getHeight method gives the height according to the conditions, meaning :math:`h_l=h_r`.
The getMomentumX method computes the method as given in the task with :math:`q_l=(hu)_l` if the stream moves to the right and :math:`q_r=-(hu)_l` if it moves to the left.

**Observations for different sets:**

    #. A larger initial water height :math:`h_l` leads to a stronger shock and a bigger jump in the solution.
    #. Increasing the magnitude of the initial velocity :math:`u_l` makes the shock formation more pronounced when the streams move toward each other.
    #. This behavior is consistent with the wave speeds :math:`\lambda_{1/2} = u \mp \sqrt{g h}`.

The Setup for the rare-rare problem was made following the given task.
The setup input is the same as in the shock-shock problem.
Thus the getHeight method is also the same.
The difference comes in the getMomentumX method, where :math:`q_l=-(hu)_l` if the stream moves left and :math:`q_r=(hu)_l` if the stream moves right.

**Observations for different sets:**

    #. If the initial velocities point away from each other, the waves spread out and a rarefaction pattern appears.
    #. The solution becomes smoother and the water height changes more gradually than in the shock-shock case.
    #. The observed behavior again matches the signs of the wave speeds :math:`\lambda_{1/2} = u \mp \sqrt{g h}`.

Tasks regarding the Dam-Break
-----------------------------

We added input variables for the momentum on the left side of the dam and the momentum on the right side.

The getMomentumX method chooses based on if the currently observed x is on the left or right side of the dam and returning the corresponding momemntum.

**Observations for different sets:**

    #. The impact of the particle velocity :math:`u_r` is very low.

**Answer to Tasks 2 of 2.2:**

The question asked is: how much time is needed for the evacuation of a village 25 km downstream of a water reservoir?
:math:`q_l=[14,0]^T` and :math:`q_r=[3.5,0.7]^T` are the given values for the problem.

Our simulation provides the answer of the wave taking 35 minutes to arrive at the village.
This was achieved by implementating a Dam-Break situation with sufficient (50000) cells and simulating the Waves untill equlibrium is reached.
From this the time of arrival was determined by finding the first time stamp in which the Schock Wave has crossed the 25000 meter mark.