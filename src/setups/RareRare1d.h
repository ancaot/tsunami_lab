/**
 * @section DESCRIPTION
 * One-dimensional rare-rare problem.
 **/
#ifndef TSUNAMI_LAB_SETUPS_RARE_RARE_1D_H
#define TSUNAMI_LAB_SETUPS_RARE_RARE_1D_H

#include "Setup.h"

namespace tsunami_lab {
	namespace setups {
		class RareRare1d;
	}
}

/**
 * 1d rare-rare setup.
 **/
class tsunami_lab::setups::RareRare1d: public Setup {
	private:
		//! water height on both sides
		t_real m_height = 0;

		//! particle velocity magnitude on the right side
		t_real m_velocityLeft = 0;

		//! location of the discontinuity
		t_real m_locationDiscontinuity = 0;

	public:
		/**
		 * Constructor.
		 *
		 * @param i_height water height on both sides.
		 * @param i_velocityLeft particle velocity of q_l (right side).
		 * @param i_locationDiscontinuity location (x-coordinate) of the discontinuity.
		 **/
		RareRare1d( t_real i_height,
								t_real i_velocityLeft,
								t_real i_locationDiscontinuity );

		/**
		 * Gets the water height at a given point.
		 *
		 * @param i_x x-coordinate of the queried point.
		 * @return water height at the given point.
		 **/
		t_real getHeight( t_real i_x,
											t_real      ) const;

		/**
		 * Gets the momentum in x-direction.
		 *
		 * @param i_x x-coordinate of the queried point.
		 * @return momentum in x-direction.
		 **/
		t_real getMomentumX( t_real i_x,
												 t_real      ) const;

		/**
		 * Gets the momentum in y-direction.
		 *
		 * @return momentum in y-direction.
		 **/
		t_real getMomentumY( t_real,
												 t_real ) const;
};

#endif
