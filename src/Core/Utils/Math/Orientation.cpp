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

	data[ 12 ] = pos[ 0 ];
	data[ 13 ] = pos[ 1 ];
	data[ 14 ] = pos[ 2 ];
}

void
Orientation::setPositionInv( const glm::vec3& pos )
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
	this->m_matrix = glm::rotate( this->m_matrix, angle, glm::vec3( 1, 0, 0 ) );
}

void
Orientation::changePitchInv( float angle )
{
	glm::mat4 mat = glm::rotate( glm::mat4( 1.0f ), angle, glm::vec3( 1, 0, 0 ) );
	this->m_matrix = mat * this->m_matrix;
}

// y-achsis rotation
void
Orientation::changeHeading( float angle )
{
	this->m_matrix = glm::rotate( this->m_matrix, angle, glm::vec3( 0, 1, 0 ) );
}

void
Orientation::changeHeadingInv( float angle )
{
	glm::mat4 mat = glm::rotate( glm::mat4( 1.0f ), angle, glm::vec3( 0, 1, 0 ) );
	this->m_matrix = mat * this->m_matrix;
}


// z-achsis rotation
void
Orientation::changeRoll( float angle )
{
	this->m_matrix = glm::rotate( this->m_matrix, angle, glm::vec3( 0, 0, 1 ) );
}

void
Orientation::changeRollInv( float angle )
{
	glm::mat4 mat = glm::rotate( glm::mat4( 1.0f ), angle, glm::vec3( 0, 0, 1 ) );
	this->m_matrix = mat * this->m_matrix;
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
Orientation::strafeForwardInv( float units )
{
	float* data = glm::value_ptr( this->m_matrix );
	data[ 14 ] += units;
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
Orientation::strafeRightInv( float units )
{
	float* data = glm::value_ptr( this->m_matrix );
	data[ 12 ] += units;
}

void
Orientation::strafeUp( float units )
{
	float* data = glm::value_ptr( this->m_matrix );

	data[ 12 ] += units * data[ 4 ];
	data[ 13 ] += units * data[ 5 ];
	data[ 14 ] += units * data[ 6 ];
}

void
Orientation::strafeUpInv( float units )
{
	float* data = glm::value_ptr( this->m_matrix );
	data[ 13 ] += units;
}
