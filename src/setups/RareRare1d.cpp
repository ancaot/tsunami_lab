/**
 * @section DESCRIPTION
 * One-dimensional rare-rare problem.
 **/
#include "RareRare1d.h"

tsunami_lab::setups::RareRare1d::RareRare1d( t_real i_height,
																						 t_real i_velocityLeft,
																						 t_real i_locationDiscontinuity ) {
	m_height = i_height;
	m_velocityLeft = i_velocityLeft;
	m_locationDiscontinuity = i_locationDiscontinuity;
}

tsunami_lab::t_real tsunami_lab::setups::RareRare1d::getHeight( t_real i_x,
																																t_real      ) const {
	(void) i_x;
	// Gleiche Wasserhoehe auf beiden Seiten: h_L = h_R = h.
	return m_height;
}

tsunami_lab::t_real tsunami_lab::setups::RareRare1d::getMomentumX( t_real i_x,
																																	 t_real      ) const {
	// Rare-rare vertauscht die Zustaende aus Gl. 2.1.1:
	// links gilt q_r = [ h, -h*u ], rechts gilt q_l = [ h, +h*u ].
	if( i_x < m_locationDiscontinuity ) {
		// Linke Seite: Impuls hu = -h*u (Stroemung nach links).
		return -m_height * m_velocityLeft;
	}
	else {
		// Rechte Seite: Impuls hu = +h*u (Stroemung nach rechts).
		return m_height * m_velocityLeft;
	}
}

tsunami_lab::t_real tsunami_lab::setups::RareRare1d::getMomentumY( t_real,
																																	 t_real ) const {
	return 0;
}
