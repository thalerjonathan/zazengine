/*
 * Orientation.h
 *
 *  Created on: Feb 6, 2011
 *      Author: jonathan
 */

#ifndef ORIENTATION_H_
#define ORIENTATION_H_

#include <glm/glm.hpp>

class Orientation
{
 public:
	Orientation( glm::mat4& );
	virtual ~Orientation();

	void setPosition( const glm::vec3& );
	void setPositionInv( const glm::vec3& );

	void changeHeading( float );
	void changeHeadingInv( float );

	void changePitch( float );
	void changePitchInv( float );

	void changeRoll( float );
	void changeRollInv( float );

	void strafeForward( float );
	void strafeForwardInv( float );

	void strafeRight( float );
	void strafeRightInv( float );

	void strafeUp( float );
	void strafeUpInv( float );

 private:
	glm::mat4& m_matrix;

};

#endif /* ORIENTATION_H_ */
