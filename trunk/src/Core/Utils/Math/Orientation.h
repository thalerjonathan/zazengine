/*
 * Orientation.h
 *
 *  Created on: Feb 6, 2011
 *      Author: jonathan
 */

#ifndef ORIENTATION_H_
#define ORIENTATION_H_

#include <glm/glm.hpp>

/* High-Level orientation class
 *
 * matrix multiplication allowes combination of multiple transformation into one matrix
 * because of the nature of matrix multiplication the order of multiplying matrices
 * with each other is important. A*B is different from B*A.
 * the transformations matrix multiplied with the last will be applied first e.g.
 * Projection * Viewing * Modeling means: modeling is applied first, viewing second
 * and projection last.
 */
// this class is designed to be used for subclassing

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

	void strafeZ( float );
	void strafeForward( float );

	void strafeX( float );
	void strafeRight( float );

	void strafeY( float );
	void strafeUp( float );

	void lookAt( const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up );

 protected:
	// this method is called always when one of the upper methods is invoded
	// this can be overridden by subclasses to react to matrix changes
	virtual void matrixChanged() {};

 private:
	glm::mat4& m_matrix;

};

#endif /* ORIENTATION_H_ */
