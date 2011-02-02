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
#include "../Material/UniformBlock.h"

#include <iostream>
#include <algorithm>

using namespace std;

DRRenderer::DRRenderer(Camera& camera, std::string& skyBoxFolder)
	: Renderer(camera, skyBoxFolder)
{
	this->m_frameBuffer = 0;

	this->m_fragShaderGeomStage = 0;
	this->m_geomStageProg = 0;
	this->m_transformBlock = 0;
	this->m_vertShaderGeomStage = 0;

	memset( this->m_mrt, sizeof( this->m_mrt), 0 );
}

DRRenderer::~DRRenderer()
{
}

bool
DRRenderer::renderFrame( std::list<Instance*>& instances )
{
	//GLenum buffers[MRT_COUNT];

	this->renderedFaces = 0;
	this->renderedInstances = 0;

	// clear window
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if ( false == this->m_transformBlock->updateData( this->camera.modelView.data, 0, 64) )
		return false;

	//  start geometry pass
	// glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->m_frameBuffer);

	// clear fbo
	//glClear(GL_COLOR_BUFFER_BIT);

	// activate drawing to targets
	/*for ( int i = 0; i < MRT_COUNT; i++)
		buffers[ i ] = GL_COLOR_ATTACHMENT0_EXT + i;
	glDrawBuffers(MRT_COUNT, buffers);
	 */

	// draw all geometry
	/*
	this->processInstance(root);

	list<GeomInstance*>::iterator iter = this->renderQueue.begin();
	while (iter != this->renderQueue.end()) {
		GeomInstance* instance = *iter++;

		if ( false == this->m_transformBlock->updateData( instance->transform.data, 64, 64) )
			return false;

		if ( false == instance->geom->render() )
			return false;
	}
*/

	this->renderQueue.clear();

	/*
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
*/

	// swap buffers
	SDL_GL_SwapBuffers();

	this->frame++;

	return true;
}

bool
DRRenderer::initialize()
{
	cout << "Initializing Deferred Renderer..." << endl;

	if ( false == this->initFBO() )
		return false;

	if ( false == this->initGeomStage() )
		return false;

	if ( false == this->initLightingStage() )
		return false;

	if ( false == this->m_geomStageProg->use() )
	{
		cout << "failed initializing Deferred Renderer - using geom-stage program failed - exit" << endl;
		return false;
	}

	if ( false == this->m_transformBlock->bind( 0 ) )
	{
		cout << "failed initializing Deferred Renderer - binding transformblock failed - exit" << endl;
		return false;
	}

	cout << "Initializing Deferred Renderer finished" << endl;

	return true;
}

bool
DRRenderer::shutdown()
{
	cout << "Shutting down Deferred Renderer..." << endl;

	if ( this->m_transformBlock )
		delete this->m_transformBlock;

	if ( this->m_geomStageProg )
	{
		if ( this->m_vertShaderGeomStage )
		{
			this->m_geomStageProg->detachShader( this->m_vertShaderGeomStage );

			delete this->m_vertShaderGeomStage;
			this->m_vertShaderGeomStage = NULL;
		}

		if ( this->m_fragShaderGeomStage )
		{
			this->m_geomStageProg->detachShader( this->m_fragShaderGeomStage );

			delete this->m_fragShaderGeomStage;
			this->m_fragShaderGeomStage = NULL;
		}

		delete this->m_geomStageProg;
		this->m_geomStageProg = NULL;
	}

	cout << "Shutting down Deferred Renderer finished" << endl;

	return true;
}

bool
DRRenderer::initFBO()
{
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

	return true;
}

bool
DRRenderer::initGeomStage()
{
	this->m_geomStageProg = Program::createProgram( );
	if ( 0 == this->m_geomStageProg )
	{
		cout << "failed initializing Deferred Renderer - coulnd't create geometry-stage program - exit" << endl;
		return false;
	}

	this->m_vertShaderGeomStage = Shader::createShader( Shader::VERTEX_SHADER, "media/graphics/dr/geomDRVert.glsl" );
	if ( 0 == this->m_vertShaderGeomStage )
	{
		cout << "failed initializing Deferred Renderer - coulnd't create geometry-stage vertex-shader - exit" << endl;
		return false;
	}

	this->m_fragShaderGeomStage = Shader::createShader( Shader::FRAGMENT_SHADER, "media/graphics/dr/geomDRFrag.glsl" );
	if ( 0 == this->m_fragShaderGeomStage )
	{
		cout << "failed initializing Deferred Renderer - coulnd't create geometry-stage fragment-shader - exit" << endl;
		return false;
	}

	this->m_transformBlock = UniformBlock::createBlock( "transform_mat" );
	if ( 0 == this->m_transformBlock )
	{
		cout << "failed initializing Deferred Renderer - creating uniform block failed - exit" << endl;
		return false;
	}

	if ( false == this->m_vertShaderGeomStage->compile() )
	{
		cout << "failed initializing Deferred Renderer - geometry-stage vertex shader compilation failed - exit" << endl;
		return false;
	}

	if ( false == this->m_fragShaderGeomStage->compile() )
	{
		cout << "failed initializing Deferred Renderer - geometry-stage fragment shader compilation failed - exit" << endl;
		return false;
	}

	if ( false == this->m_geomStageProg->attachShader( this->m_vertShaderGeomStage ) )
	{
		cout << "failed initializing Deferred Renderer - attaching vertex shader to geom-stage program failed - exit" << endl;
		return false;
	}

	if ( false == this->m_geomStageProg->attachShader( this->m_fragShaderGeomStage ) )
	{
		cout << "failed initializing Deferred Renderer - attaching fragment shader to geom-stage program failed - exit" << endl;
		return false;
	}

	// setting frag-data location is done bevore linking
	/*
	this->m_geomStageProg->bindFragDataLocation( 0, "out_diffuse" );
	this->m_geomStageProg->bindFragDataLocation( 1, "out_normal" );
	this->m_geomStageProg->bindFragDataLocation( 2, "out_depth" );
	this->m_geomStageProg->bindFragDataLocation( 3, "out_generic" );
	*/

	if ( false == this->m_geomStageProg->bindAttribLocation( 0, "in_vertPos" ) )
	{
		cout << "failed initializing Deferred Renderer - binding attribute location for geom-stage program failed - exit" << endl;
		return false;
	}

	if ( false == this->m_geomStageProg->bindAttribLocation( 1, "in_vertNorm" ) )
	{
		cout << "failed initializing Deferred Renderer - binding attribute location for geom-stage program failed - exit" << endl;
		return false;
	}

	if ( false == this->m_geomStageProg->link() )
	{
		cout << "failed initializing Deferred Renderer - linking geom-stage program failed - exit" << endl;
		return false;
	}

	if ( false == this->m_geomStageProg->bindUniformBlock( this->m_transformBlock ) )
	{
		cout << "failed initializing Deferred Renderer - failed binding uniform block - exit" << endl;
		return false;
	}

	return true;
}

bool
DRRenderer::initLightingStage()
{
	return true;
}
