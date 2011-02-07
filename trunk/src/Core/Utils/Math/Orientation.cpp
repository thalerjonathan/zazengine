/*
 * Orientation.cpp
 *
 *  Created on: Feb 6, 2011
 *      Author: jonathan
 */

#include "Orientation.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Orientation::Orientation( glm::mat4& mat )
	: m_matrix( mat )
{
}

Orientation::~Orientation()
{
	// don't delete m_matrix because orientation doesn't own it
}

void
Orientation::setPosition( const glm::vec3& pos )
{
	float* data = glm::value_ptr( this->m_matrix );

	data[ 12 ] = -pos[ 0 ];
	data[ 13 ] = -pos[ 1 ];
	data[ 14 ] = -pos[ 2 ];
}

// x-achsis rotation
void
Orientation::changePitch( float angle )
{
	glm::vec3 axis( 1, 0, 0 );
	this->m_matrix = glm::rotate( this->m_matrix, angle, axis );
}

// y-achsis rotation
void
Orientation::changeHeading( float angle )
{
	glm::vec3 axis( 0, 1, 0 );
	this->m_matrix = glm::rotate( this->m_matrix, angle, axis );
}

// z-achsis rotation
void
Orientation::changeRoll( float angle )
{
	glm::vec3 axis( 0, 0, 1 );
	this->m_matrix = glm::rotate( this->m_matrix, angle, axis );
}

void
Orientation::strafeForward( float units )
{
	float* data = glm::value_ptr( this->m_matrix );

	data[ 12 ] += units * data[ 8 ];
	data[ 13 ] += units * data[ 9 ];
	data[ 14 ] += units * data[ 10 ];
}

void
Orientation::strafeRight( float units )
{
	float* data = glm::value_ptr( this->m_matrix );

	data[ 12 ] += units * data[ 0 ];
	data[ 13 ] += units * data[ 1 ];
	data[ 14 ] += units * data[ 2 ];
}

void
Orientation::strafeUp( float units )
{
	float* data = glm::value_ptr( this->m_matrix );

	data[ 12 ] += units * data[ 4 ];
	data[ 13 ] += units * data[ 5 ];
	data[ 14 ] += units * data[ 6 ];
}
