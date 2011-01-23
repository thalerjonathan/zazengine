/*
 * DRRenderer.cpp
 *
 *  Created on: Jan 16, 2011
 *      Author: jonathan
 */

#include <GL/glew.h>
#include "SDL/SDL.h"
#include <glm/glm.hpp>

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
	GLenum buffers[MRT_COUNT];

	this->renderedFaces = 0;
	this->renderedInstances = 0;

	// clear window
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// load modelviewmatrix
	glLoadMatrixf(this->camera.modelView.data);

	//  start geometry pass
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->m_frameBuffer);

	// clear fbo
	glClear(GL_COLOR_BUFFER_BIT);

	// activate drawing to targets
	for ( int i = 0; i < MRT_COUNT; i++)
		buffers[ i ] = GL_COLOR_ATTACHMENT0_EXT + i;
	glDrawBuffers(MRT_COUNT, buffers);

	this->m_geomStageProg->activate();

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glColor4f(1, 0, 0, 0);
	glBegin(GL_LINES);
		glVertex3f(100, 0, 0);
		glVertex3f(-100, 0, 0);
	glEnd();

	glColor4f(0, 1, 0, 0);
	glBegin(GL_LINES);
		glVertex3f(0, 100, 0);
		glVertex3f(0, -100, 0);
	glEnd();

	glColor4f(0, 0, 1, 0);
	glBegin(GL_LINES);
		glVertex3f(0, 0, 100);
		glVertex3f(0, 0, -100);
	glEnd();

	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);

	// draw all geometry
	this->processInstance(root);
	this->processRenderQueue( false );

	// finish geometry stage
	this->m_geomStageProg->deactivate();

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	// start lighting stage
	// bind rendertargets as textures
	for ( int i = 0; i < MRT_COUNT; i++ )
	{
		glActiveTexture( GL_TEXTURE0 + i );
		glBindTexture( GL_TEXTURE_2D, this->m_mrt[ i ]);
	}

	// activate lighting-stage shader
	//this->m_lightStageProg->activate();

	// set up orthogonal projection to render quad
	this->camera.setupOrtho();

	// render quad
	glBegin( GL_QUADS );
		glTexCoord2f( 0.0f, 0.0f ); glVertex2f( 0, 0 );
		glTexCoord2f( 0.0f, 1.0f ); glVertex2f( 0, this->camera.getHeight() );
		glTexCoord2f( 1.0f, 1.0f ); glVertex2f( this->camera.getWidth(), this->camera.getHeight() );
		glTexCoord2f( 1.0f, 0.0f ); glVertex2f( this->camera.getWidth(), 0 );
	glEnd();

	// finish lighting stage
	//this->m_lightStageProg->deactivate();

	// switch back to perspective projection
	this->camera.setupPerspective();

	// swap buffers
	SDL_GL_SwapBuffers();

	this->frame++;
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
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT + i, GL_TEXTURE_RECTANGLE_ARB, this->m_mrt[i], 0);

		CHECK_FRAMEBUFFER_STATUS( status )
		if ( GL_FRAMEBUFFER_COMPLETE_EXT != status )
		{
			cout << "framebuffer error: " << gluErrorString(glGetError()) << " - exit" << endl;
			return false;
		}

		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	}

	this->m_geomStageProg = Program::createProgram( "media/graphics/dr/geomDRVert.glsl", "media/graphics/dr/geomDRFrag.glsl" );
	if ( 0 == this->m_geomStageProg )
	{
		cout << "failed initializing Deferred Renderer - coulnd't create geometry-stage program - exit" << endl;
		return false;
	}

	this->m_geomStageProg->bindFragDataLocation( 0, "normalOut" );
	this->m_geomStageProg->bindFragDataLocation( 1, "diffuseOut" );
	this->m_geomStageProg->bindFragDataLocation( 2, "depthOut" );
	this->m_geomStageProg->bindFragDataLocation( 3, "genericOut" );

	if ( false == this->m_geomStageProg->link() )
	{
		return false;
	}

	this->m_lightStageProg = Program::createProgram( "media/graphics/dr/lightDRVert.glsl", "media/graphics/dr/lightDRFrag.glsl" );
	if ( 0 == this->m_lightStageProg )
	{
		cout << "failed initializing Deferred Renderer - coulnd't create geometry-stage program - exit" << endl;
		return false;
	}

	if ( false == this->m_lightStageProg->link() )
	{
		return false;
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
