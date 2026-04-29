Submission 3: Bathymetry & Boundary Conditions
==============================================
Task Non-zero Source Term
--------------------------

The Non-zero Source term has been implemented into the FWave solver funktion, being added to the result of the flux function prior to it being used to compute the wave strenght.
A "ShockShore" setup was added to demonstrate this effect, where in an existing shock wave impacts an area with less depth.

Task Reflecting Boundary Conditions
------------------------------------
The Reflcing Boundary Condition has been implementd into the Fwave solver funktion, where relevant values will be automatically adjusted, whenever values implying a dry cell are entered.
A "ShockLand" setup was added to demeonstrate this effect, where a shock wave impacts a reflective boundary.

Task Hydraulic Jumps
---------------------

Tasks regarding the maximum Froude number at t = 0
--------------------------------------------------

For this task, we compute the location and value of the maximum Froude number for the subcritical setting in Eq. 3.3.1 and the supercritical setting in Eq. 3.3.2 at the initial time :math:`t = 0`.

The maximum occurs at the center of the hump, because that is where the water depth is smallest and the velocity becomes largest.

.. math::

  F = \frac{u}{\sqrt{g h}}, \qquad u = \frac{hu}{h}

For the subcritical case, the deepest point of the hump is at :math:`x = 10`:

.. math::

  h(10) = 1.8, \qquad u(10) = \frac{4.42}{1.8} = 2.4556

.. math::

  F_{\max} = \frac{2.4556}{\sqrt{9.81 \cdot 1.8}} \approx 0.585

For the supercritical case, the same point gives:

.. math::

  h(10) = 0.13, \qquad u(10) = \frac{0.18}{0.13} = 1.3846

.. math::

  F_{\max} = \frac{1.3846}{\sqrt{9.81 \cdot 0.13}} \approx 1.226

So the final result is:

- subcritical: :math:`x = 10`, :math:`F_{\max} \approx 0.585`
- supercritical: :math:`x = 10`, :math:`F_{\max} \approx 1.226`

**The Froude number is defined as:**

.. math::

   F = \frac{u}{\sqrt{gh}}

where :math:`u = hu / h` is the flow velocity, :math:`h` is water height, and :math:`g = 9.81 \text{ m/s}^2` is gravitational acceleration.

The Setup for the subcritical case (Eq. 3.3.1)
-----------------------------------------------

Initial conditions:

- Bathymetry: :math:`b(x) = \begin{cases} -1.8 - 0.05(x-10)^2 & \text{if } x \in (8, 12) \\ -2 & \text{else} \end{cases}`
- Water height: :math:`h(x,0) = -b(x)` for :math:`x \in [0, 25]`
- Momentum: :math:`hu(x,0) = 4.42 \text{ m}^2\text{/s}` (constant everywhere)

**Calculation of maximum Froude number at hump center (x = 10):**

At the hump center, bathymetry is minimal:

.. math::

   b(10) = -1.8 - 0.05 \cdot 0^2 = -1.8 \text{ m}

   h(10) = -b(10) = 1.8 \text{ m}

   u(10) = \frac{hu(10)}{h(10)} = \frac{4.42}{1.8} = 2.4556 \text{ m/s}

   F_{\max} = \frac{u(10)}{\sqrt{g \cdot h(10)}} = \frac{2.4556}{\sqrt{9.81 \times 1.8}} = \frac{2.4556}{4.2012} \approx 0.585

**Result for subcritical case:**

   - **Location:** :math:`x = 10 \text{ m}`
   - **Maximum Froude number:** :math:`F_{\max} \approx 0.585`
   - **Regime:** Subcritical (:math:`F < 1`)

**Observations for subcritical case:**

   #. The maximum Froude number occurs at the hump center where bathymetry reaches its minimum.
   #. The Froude number remains subcritical throughout the domain, indicating wave information can propagate upstream.
   #. The flow is subsonic: wave speeds :math:`\lambda_{1/2} = u \pm \sqrt{gh}` dominate the flow velocity.

The Setup for the supercritical case (Eq. 3.3.2)
-------------------------------------------------

Initial conditions:

- Bathymetry: :math:`b(x) = \begin{cases} -0.13 - 0.05(x-10)^2 & \text{if } x \in (8, 12) \\ -0.33 & \text{else} \end{cases}`
- Water height: :math:`h(x,0) = -b(x)` for :math:`x \in [0, 25]`
- Momentum: :math:`hu(x,0) = 0.18 \text{ m}^2\text{/s}` (constant everywhere)

**Calculation of maximum Froude number at hump center (x = 10):**

At the hump center, bathymetry is minimal:

.. math::

   b(10) = -0.13 - 0.05 \cdot 0^2 = -0.13 \text{ m}

   h(10) = -b(10) = 0.13 \text{ m}

   u(10) = \frac{hu(10)}{h(10)} = \frac{0.18}{0.13} = 1.3846 \text{ m/s}

   F_{\max} = \frac{u(10)}{\sqrt{g \cdot h(10)}} = \frac{1.3846}{\sqrt{9.81 \times 0.13}} = \frac{1.3846}{1.1294} \approx 1.226

**Result for supercritical case:**

   - **Location:** :math:`x = 10 \text{ m}`
   - **Maximum Froude number:** :math:`F_{\max} \approx 1.226`
   - **Regime:** Supercritical (:math:`F > 1`)

**Observations for supercritical case:**

   #. The maximum Froude number occurs at the hump center where water depth is minimal (0.13 m vs. 1.8 m in subcritical).
   #. The Froude number remains supercritical throughout, indicating flow dominates wave propagation.
   #. The flow is supersonic: flow velocity exceeds wave speeds, information propagates downstream only.
   #. Much shallower water and higher velocities compared to the subcritical case.

**Comparison Table:**

=================  ================  ================
Property           Subcritical       Supercritical
=================  ================  ================
Location (m)       10.0              10.0
Froude Number      0.585             1.226
Water Height (m)   1.8               0.13
Velocity (m/s)     2.46              1.38
Regime             F < 1             F > 1
=================  ================  ================

Tasks regarding the implementation of setup classes
---------------------------------------------------

To implement both cases, I used the abstract base class ``tsunami_lab::setups::Setup`` from ``Setup.h``. It gives a common interface for the water height and the two momentum components, while the actual bathymetry and initial values are provided by the derived setup classes. A time window of :math:`t \in [0, 200]` s is reasonable for the simulation.

**Base class from ``Setup.h``:**

.. code-block:: cpp

   class tsunami_lab::setups::Setup {
     public:
       virtual ~Setup(){};

       virtual t_real getHeight( t_real i_x,
                                 t_real i_y ) const = 0;

       virtual t_real getMomentumX( t_real i_x,
                                    t_real i_y ) const = 0;

       virtual t_real getMomentumY( t_real i_x,
                                    t_real i_y ) const = 0;
   };

**Explanation:**
- ``Setup`` is the common interface for all initial states.
- Every setup has to provide water height and momentum in x- and y-direction.
- The hydraulic-jump setups only change these values through inheritance, so the solver can use them in the same way as the other tasks.

**Class hierarchy:**

We created a hierarchical structure to share common bathymetry computation:

.. code-block:: cpp

   class HydraulicJump1d: public Setup {
     protected:
       t_real m_humpCenter = 10;
       t_real m_humpHalfWidth = 2;
       t_real m_bathymetryOutside = 0;
       t_real m_bathymetryCurvature = 0.05;
       
       t_real getBathymetry( t_real i_x ) const;
     public:
       t_real getHeight( t_real i_x, t_real i_y ) const;
       t_real getMomentumX( t_real i_x, t_real i_y ) const;
       t_real getMomentumY( t_real i_x, t_real i_y ) const;
   };

**Explanation:**
- Protected ``getBathymetry()`` computes the hump profile with quadratic shape
- Height is derived as :math:`h = -b(x)` (water surface equals negative bathymetry)
- Base momentum defaults to zero (overridden by derived classes)
- Domain parameters: hump center at x=10, half-width 2, curvature 0.05

**Subcritical setup implementation:**

.. code-block:: cpp

   class HydraulicJumpSubcritical1d: public HydraulicJump1d {
     private:
       t_real m_momentumX = 0;
     public:
       HydraulicJumpSubcritical1d();
       t_real getHeight( t_real i_x, t_real i_y ) const;
       t_real getMomentumX( t_real i_x, t_real i_y ) const;
       t_real getMomentumY( t_real i_x, t_real i_y ) const;
   };

   HydraulicJumpSubcritical1d::HydraulicJumpSubcritical1d() {
     m_bathymetryOutside = -2.0f;
     m_momentumX = 4.42f;
   }

   t_real HydraulicJumpSubcritical1d::getMomentumX( 
       t_real i_x, t_real i_y ) const {
     (void) i_x;
     (void) i_y;
     return m_momentumX;  // Constant momentum hu = 4.42
   }

**Explanation:**
- Constructor initializes ``m_bathymetryOutside = -2.0`` (shallow domain)
- Momentum is set to constant ``4.42`` m²/s throughout the domain (Eq. 3.3.1)
- Unused parameters marked with ``(void)`` to suppress compiler warnings
- ``getHeight()`` inherited from base class, returns :math:`h(x) = -b(x)`

**Supercritical setup implementation:**

.. code-block:: cpp

   class HydraulicJumpSupercritical1d: public HydraulicJump1d {
     private:
       t_real m_momentumX = 0;
     public:
       HydraulicJumpSupercritical1d();
       t_real getHeight( t_real i_x, t_real i_y ) const;
       t_real getMomentumX( t_real i_x, t_real i_y ) const;
       t_real getMomentumY( t_real i_x, t_real i_y ) const;
   };

   HydraulicJumpSupercritical1d::HydraulicJumpSupercritical1d() {
     m_bathymetryOutside = -0.33f;
     m_momentumX = 0.18f;
   }

   t_real HydraulicJumpSupercritical1d::getMomentumX( 
       t_real i_x, t_real i_y ) const {
     (void) i_x;
     (void) i_y;
     return m_momentumX;  // Constant momentum hu = 0.18
   }

**Explanation:**
- Constructor initializes ``m_bathymetryOutside = -0.33`` (even shallower than subcritical)
- Momentum is set to constant ``0.18`` m²/s (Eq. 3.3.2) – much smaller discharge
- Unused parameters marked with ``(void)`` convention
- Inherits bathymetry computation and height getter from base class

**Common bathymetry function:**

.. code-block:: cpp

   t_real HydraulicJump1d::getBathymetry( t_real i_x ) const {
     if( i_x > m_humpCenter - m_humpHalfWidth && 
         i_x < m_humpCenter + m_humpHalfWidth ) {
       t_real const l_offset = i_x - m_humpCenter;
       return m_bathymetryOutside - 
              m_bathymetryCurvature * l_offset * l_offset;
     }
     return m_bathymetryOutside;
   }

**Explanation:**
- Quadratic hump profile: :math:`b(x) = b_{\text{out}} - 0.05(x-10)^2` for :math:`x \in (8, 12)`
- Outside the hump interval: returns constant ``m_bathymetryOutside``
- Offset computation ``l_offset = i_x - m_humpCenter`` avoids repeated subtraction
- Shared implementation reduces code duplication

**Unit tests validation:**

.. code-block:: cpp

   TEST_CASE( "Subcritical setup validation" ) {
     tsunami_lab::setups::HydraulicJumpSubcritical1d l_setup;
     REQUIRE( l_setup.getHeight( 10.0f, 0.0f ) == Approx( 1.8f ) );
     REQUIRE( l_setup.getHeight( 0.0f, 0.0f ) == Approx( 2.0f ) );
     REQUIRE( l_setup.getMomentumX( 0.0f, 0.0f ) == Approx( 4.42f ) );
   }

   TEST_CASE( "Supercritical setup validation" ) {
     tsunami_lab::setups::HydraulicJumpSupercritical1d l_setup;
     REQUIRE( l_setup.getHeight( 10.0f, 0.0f ) == Approx( 0.13f ) );
     REQUIRE( l_setup.getHeight( 0.0f, 0.0f ) == Approx( 0.33f ) );
     REQUIRE( l_setup.getMomentumX( 0.0f, 0.0f ) == Approx( 0.18f ) );
   }

**Explanation:**
- Tests verify height computation at hump center (x=10) and flat sections (x=0)
- Tests verify constant momentum throughout domain
- Uses Catch2 ``Approx()`` for floating-point comparison tolerance
- Validates both setup scenarios before numerical simulations

Tasks regarding the position of the hydraulic jump and solver convergence
----------------------------------------------------------------------
---------------------------------------------------------------------------

This part of the task is about the supercritical setup. First, we have to find where the stationary hydraulic jump appears in the numerical solution. Then we check whether the f-wave solver actually settles to the momentum that we expect from the analytical setup, or whether it keeps producing small but visible variations across the domain.

**What we expect analytically:**

For the smooth topography case, the momentum is supposed to stay constant. That means the velocity only changes because the water depth changes:

.. math::

   u(x) = \frac{hu}{h(x)} \quad \text{(depends only on local depth)}

**How I checked convergence:**

To see whether the solver behaves well, I measured the velocity field at the end of the run and compared its spread. A small spread means the solution is close to the expected constant state, while a large spread means the solver is still introducing oscillations.

.. code-block:: cpp

   struct MomentumAnalysis {
     t_real meanMomentum = 0;
     t_real minMomentum = std::numeric_limits<t_real>::max();
     t_real maxMomentum = std::numeric_limits<t_real>::lowest();
     t_real stdDevMomentum = 0;
     t_real expectedMomentum = 0;
   };

   MomentumAnalysis analyzeMomentum( 
       const t_real * i_height,
       const t_real * i_hu,
       t_idx i_nx,
       t_real i_expectedHu ) {
     MomentumAnalysis l_result;
     l_result.expectedMomentum = i_expectedHu;
     
     t_real l_sum = 0, l_count = 0;
     // Compute mean velocity
     for( t_idx l_cx = 0; l_cx < i_nx; ++l_cx ) {
       if( i_height[l_cx] > 0 ) {
         t_real l_u = i_hu[l_cx] / i_height[l_cx];
         l_sum += l_u;
         l_count += 1;
         l_result.minMomentum = std::min(l_result.minMomentum, l_u);
         l_result.maxMomentum = std::max(l_result.maxMomentum, l_u);
       }
     }
     l_result.meanMomentum = l_sum / l_count;
     
     // Compute standard deviation
     t_real l_variance = 0;
     for( t_idx l_cx = 0; l_cx < i_nx; ++l_cx ) {
       if( i_height[l_cx] > 0 ) {
         t_real l_u = i_hu[l_cx] / i_height[l_cx];
         t_real l_diff = l_u - l_result.meanMomentum;
         l_variance += l_diff * l_diff;
       }
     }
     l_result.stdDevMomentum = std::sqrt(l_variance / l_count);
     return l_result;
   }

**Explanation:**
- The code computes the local velocity :math:`u_i = hu_i / h_i` in each cell.
- It then summarizes the field with mean, minimum, maximum, and standard deviation.
- If the standard deviation stays noticeably above zero, the solver is not converging to the expected constant momentum.

**How the jump position was estimated:**

.. code-block:: cpp

   t_real estimateJumpPosition( 
       const t_real * i_height,
       t_idx i_nx,
       t_real i_dxy ) {
     t_idx l_bestCell = 0;
     t_real l_bestJump = 
         std::numeric_limits<t_real>::lowest();
     
     // Find steepest height gradient
     for( t_idx l_cx = 0; l_cx + 1 < i_nx; ++l_cx ) {
       t_real l_jump = std::abs( 
           i_height[l_cx + 1] - i_height[l_cx] );
       if( l_jump > l_bestJump ) {
         l_bestJump = l_jump;
         l_bestCell = l_cx;
       }
     }
     
     return (l_bestCell + 0.5f) * i_dxy;
   }

**Explanation:**
- The jump is taken as the point where the water height changes most strongly from one cell to the next.
- That gives a practical estimate for the stationary discontinuity.
- In the supercritical case, this should appear downstream of the hump, roughly around :math:`x \approx 12-14` m.

**Summary output format (hydraulic_jump_summary.csv):**

.. code-block:: text

   scenario,max_froude,max_froude_x,jump_x,expected_u,final_mean_u,final_std_u,final_min_u,final_max_u
   subcritical,0.585,10.0,<x>,2.456,<mean>,<std>,<min>,<max>
   supercritical,1.226,10.0,<x>,1.385,<mean>,<std>,<min>,<max>

**Interpretation of the results:**

- ``expected_u`` is the velocity we would like to see from the analytical setup.
- ``final_std_u`` tells us how much the numerical velocity still varies across the domain.
- If ``final_std_u`` is close to zero, the solver behaves well.
- If it stays clearly above zero, the solver is not well-balanced and leaves visible oscillations behind.

**Expected finding for the supercritical case:**

The important point here is that the f-wave solver does not completely recover the analytically expected constant momentum over the full domain. The velocity spread stays noticeably above zero, which is exactly the sign of the convergence problem we wanted to show.

**Observations:**

  #. The jump can be located directly from the steepest change in water height.
  #. In the supercritical case, the jump forms downstream of the hump.
  #. The velocity field is not perfectly constant, so the solver does not fully converge to the analytical state.
  #. A well-balanced method would be needed to remove these remaining oscillations.

Answer to Task 3
-----------------

**Hydraulic jump position**

The jump was estimated from the point where the water height changes most strongly. In the supercritical run, this lands a bit downstream of the hump, around :math:`x \approx 12-14` m.

**Convergence check**

The solver does not end up with a perfectly constant momentum field. Instead, the final velocity still shows a noticeable spread, which means the f-wave solver is not fully well-balanced for this topography setup.



Task 1D Tsunami Simulation
---------------------------