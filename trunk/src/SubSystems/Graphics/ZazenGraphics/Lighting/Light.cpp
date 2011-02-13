/*
 * Light.cpp
 *
 *  Created on: Feb 4, 2011
 *      Author: jonathan
 */

#include "Light.h"

#include <iostream>

using namespace std;

Light*
Light::createLight( float angle, int width, int height )
{
	GLenum status;
	GLuint shadowMapID;

	// Try to use a texture depth component
	glGenTextures( 1, &shadowMapID );
	if ( GL_NO_ERROR != ( status = glGetError() ) )
	{
		cout << "ERROR in Light::createLight: glGenTextures failed with " << gluErrorString( status ) << " - exit" << endl;
		return false;
	}

	glBindTexture( GL_TEXTURE_2D, shadowMapID );
	if ( GL_NO_ERROR != ( status = glGetError() ) )
	{
		cout << "ERROR in Light::createLight: glBindTexture failed with " << gluErrorString( status ) << " - exit" << endl;
		return false;
	}

	// GL_LINEAR does not make sense for depth texture. However, next tutorial shows usage of GL_LINEAR and PCF
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

	// Remove artifact on the edges of the shadowmap
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );

	// need to enable comparison-mode for depth-texture to use it as a shadow2DSampler in shader
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL );

	// for now we create shadowmaps in same width and height as their viewing frustum
	glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0 );
	if ( GL_NO_ERROR != ( status = glGetError() ) )
	{
		cout << "ERROR in Light::createLight: glTexImage2D failed with " << gluErrorString( status ) << " - exit" << endl;
		return false;
	}

	// unbind framebuffer depth-target
	glBindTexture( GL_TEXTURE_2D, 0 );

	Light* light = new Light( angle, width, height );
	light->m_shadowMapID = shadowMapID;

	return light;
}

Light::Light( float angle, int width, int height )
	: Viewer( angle, width, height )
{
	this->m_shadowMapID = 0;
}

Light::~Light()
{
	if ( this->m_shadowMapID )
		glDeleteTextures( 1, &this->m_shadowMapID );
}
