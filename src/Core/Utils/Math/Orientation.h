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

	void changeHeading( float );
	void changePitch( float );
	void changeRoll( float );
	void strafeForward( float );
	void strafeRight( float );
	void strafeUp( float );

 private:
	glm::mat4& m_matrix;

};

#endif /* ORIENTATION_H_ */
