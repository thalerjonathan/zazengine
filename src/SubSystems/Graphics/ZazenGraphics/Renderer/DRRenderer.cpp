/*
 * DRRenderer.cpp
 *
 *  Created on: Jan 16, 2011
 *      Author: jonathan
 */

#include <GL/glew.h>
#include "SDL/SDL.h"

#include "DRRenderer.h"

#include <iostream>
#include <algorithm>

using namespace std;

DRRenderer::DRRenderer(Camera& camera, std::string& skyBoxFolder)
	: Renderer(camera, skyBoxFolder)
{
	this->m_frameBuffer = 0;

	memset( this->m_mrt, sizeof( this->m_mrt), 0 );
}

DRRenderer::~DRRenderer()
{
}

void
DRRenderer::renderFrame(GeomInstance* root)
{
	// 1. do geometry pass
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->m_frameBuffer);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

bool
DRRenderer::initialize()
{
	cout << "Initializing Deferred Renderer..." << endl;

	GLenum status;
	glGenFramebuffersEXT(1, &this->m_frameBuffer);

	for ( int i = 0; i < MRT_COUNT; i++ )
	{
		glGenTextures(1, &this->m_mrt[i]);

		if ( glGetError() != GL_NO_ERROR )
		{
			cout << "glGenTextures failed with " << gluErrorString(glGetError()) << " - exit" << endl;
			return false;
		}

		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->m_frameBuffer);
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, this->m_mrt[i]);
		glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA8, this->camera.getWidth(), this->camera.getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

		if (glGetError() != GL_NO_ERROR)
		{
			cout << "glTexImage2D failed with " << gluErrorString(glGetError()) << " - exit" << endl;
			return false;
		}

		glClearColor(0, 0, 0, 0);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_RECTANGLE_ARB, this->m_mrt[i], 0);

		CHECK_FRAMEBUFFER_STATUS( status )
		if ( GL_FRAMEBUFFER_COMPLETE_EXT != status )
		{
			cout << "framebuffer error: " << gluErrorString(glGetError()) << " - exit" << endl;
			return false;
		}

		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	}

	cout << "Initializing Deferred Renderer finished" << endl;

	return true;
}

bool
DRRenderer::shutdown()
{
	cout << "Shutting down Deferred Renderer..." << endl;

	// TODO: cleanup

	cout << "Shutting down Deferred Renderer finished" << endl;

	return true;
}
