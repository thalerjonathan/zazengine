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
	memset( this->m_mrt, sizeof( this->m_mrt), 0 );

	this->m_progGeomStage = 0;
	this->m_vertShaderGeomStage = 0;
	this->m_fragShaderGeomStage = 0;

	this->m_progLightingStage = 0;
	this->m_vertShaderLightingStage = 0;
	this->m_fragShaderLightingStage = 0;

	this->m_progShadowMapping = 0;
	this->m_vertShaderhadowMapping = 0;
	this->m_fragShaderhadowMapping = 0;

	this->m_transformBlock = 0;

	this->m_light = 0;
}

DRRenderer::~DRRenderer()
{
}

bool
DRRenderer::renderFrame( std::list<Instance*>& instances )
{
	//GLenum buffers[MRT_COUNT];

	// clear window
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	Matrix mvp( this->camera.modelView.data );
	mvp.multiply( this->camera.projection  );

	if ( false == this->m_transformBlock->updateData( mvp.data , 0, 64) )
		return false;

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
	list<Instance*>::iterator iter = instances.begin();
	while ( iter != instances.end() )
	{
		Instance* instance = *iter++;

		Matrix transf( instance->transform.matrix.data );
		transf.multiply( mvp );

		this->renderGeom( transf, instance->geom );
	}

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

	//if ( false == this->initFBO() )
	//	return false;

	if ( false == this->initGeomStage() )
		return false;

	if ( false == this->initLightingStage() )
		return false;

	if ( false == this->initShadowMapping() )
		return false;

	if ( false == this->m_progGeomStage->use() )
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

	if ( this->m_progGeomStage )
	{
		if ( this->m_vertShaderGeomStage )
		{
			this->m_progGeomStage->detachShader( this->m_vertShaderGeomStage );

			delete this->m_vertShaderGeomStage;
			this->m_vertShaderGeomStage = NULL;
		}

		if ( this->m_fragShaderGeomStage )
		{
			this->m_progGeomStage->detachShader( this->m_fragShaderGeomStage );

			delete this->m_fragShaderGeomStage;
			this->m_fragShaderGeomStage = NULL;
		}

		delete this->m_progGeomStage;
		this->m_progGeomStage = NULL;
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
	cout << "Initializing Deferred Rendering Geometry-Stage..." << endl;

	this->m_progGeomStage = Program::createProgram( );
	if ( 0 == this->m_progGeomStage )
	{
		cout << "failed initializing Deferred Renderer - coulnd't create geometry-stage program - exit" << endl;
		return false;
	}

	this->m_vertShaderGeomStage = Shader::createShader( Shader::VERTEX_SHADER, "media/graphics/dr/stages/geom/geomVert.glsl" );
	if ( 0 == this->m_vertShaderGeomStage )
	{
		cout << "failed initializing Deferred Renderer - coulnd't create geometry-stage vertex-shader - exit" << endl;
		return false;
	}

	this->m_fragShaderGeomStage = Shader::createShader( Shader::FRAGMENT_SHADER, "media/graphics/dr/stages/geom/geomFrag.glsl" );
	if ( 0 == this->m_fragShaderGeomStage )
	{
		cout << "failed initializing Deferred Renderer - coulnd't create geometry-stage fragment-shader - exit" << endl;
		return false;
	}

	this->m_transformBlock = UniformBlock::createBlock( "transform" );
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

	if ( false == this->m_progGeomStage->attachShader( this->m_vertShaderGeomStage ) )
	{
		cout << "failed initializing Deferred Renderer - attaching vertex shader to geom-stage program failed - exit" << endl;
		return false;
	}

	if ( false == this->m_progGeomStage->attachShader( this->m_fragShaderGeomStage ) )
	{
		cout << "failed initializing Deferred Renderer - attaching fragment shader to geom-stage program failed - exit" << endl;
		return false;
	}

	// setting frag-data location is done bevore linking
	/*
	this->m_progGeomStage->bindFragDataLocation( 0, "out_diffuse" );
	this->m_progGeomStage->bindFragDataLocation( 1, "out_normal" );
	this->m_progGeomStage->bindFragDataLocation( 2, "out_depth" );
	this->m_progGeomStage->bindFragDataLocation( 3, "out_generic" );
	*/

	if ( false == this->m_progGeomStage->bindAttribLocation( 0, "in_vertPos" ) )
	{
		cout << "failed initializing Deferred Renderer - binding attribute location for geom-stage program failed - exit" << endl;
		return false;
	}

	if ( false == this->m_progGeomStage->bindAttribLocation( 1, "in_vertNorm" ) )
	{
		cout << "failed initializing Deferred Renderer - binding attribute location for geom-stage program failed - exit" << endl;
		return false;
	}

	if ( false == this->m_progGeomStage->link() )
	{
		cout << "failed initializing Deferred Renderer - linking geom-stage program failed - exit" << endl;
		return false;
	}

	if ( false == this->m_progGeomStage->bindUniformBlock( this->m_transformBlock ) )
	{
		cout << "failed initializing Deferred Renderer - failed binding uniform block - exit" << endl;
		return false;
	}

	cout << "Initializing Deferred Rendering Geometry-Stage finished" << endl;

	return true;
}

bool
DRRenderer::initLightingStage()
{
	cout << "Initializing Deferred Rendering Lighting-Stage..." << endl;

	this->m_light = new Light();

	this->m_light->viewingTransf.setPosition( 0, 500, 0 );

	cout << "Initializing Deferred Rendering Lighting-Stage finished" << endl;

	return true;
}

bool
DRRenderer::initShadowMapping()
{
	cout << "Initializing Deferred Rendering Shadow-Mapping..." << endl;

	cout << "Initializing Deferred Rendering Shadow-Mapping finished" << endl;

	return true;
}

bool
DRRenderer::renderGeom( Matrix& transf, GeomType* geom )
{
	if ( geom->children.size() )
	{
		for ( unsigned int i = 0; i < geom->children.size(); i++ )
		{
			return this->renderGeom( transf, geom->children[ i ] );
		}
	}
	else
	{
		Matrix mat( geom->model_transf );
		mat.multiply( transf );

		if ( false == this->m_transformBlock->updateData( mat.data, 0, 64) )
			return false;

		return geom->render();
	}

	return true;
}
