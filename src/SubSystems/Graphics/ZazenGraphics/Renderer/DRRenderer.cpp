/*
 * DRRenderer.cpp
 *
 *  Created on: Jan 16, 2011
 *      Author: jonathan
 */

#include <GL/glew.h>
#include "SDL/SDL.h"

#include <glm/gtc/type_ptr.hpp>

#include "DRRenderer.h"
#include "../Material/UniformBlock.h"

#include <iostream>
#include <algorithm>

using namespace std;

DRRenderer::DRRenderer( Viewer* camera )
	: Renderer( camera )
{
	this->m_drFB = 0;
	memset( this->m_mrt, 0, sizeof( this->m_mrt) );

	this->m_progGeomStage = 0;
	this->m_vertGeomStage = 0;
	this->m_fragGeomStage = 0;

	this->m_progLightingStage = 0;
	this->m_vertLightingStage = 0;
	this->m_fragLightingStage = 0;

	this->m_progShadowMapping = 0;
	this->m_vertShadowMapping = 0;
	this->m_fragShadowMapping = 0;

	this->m_shadowMap = 0;
	this->m_shadowMappingFB = 0;

	this->m_mvpTransformBlock = 0;
	this->m_lightDataBlock = 0;

	this->m_light = 0;

	float unitCube[] = {
			0.5, 0.0, 0.0, 0.0,
			0.0, 0.5, 0.0, 0.0,
			0.0, 0.0, 0.5, 0.0,
		0.5, 0.5, 0.5, 1.0};

	memcpy( glm::value_ptr( this->m_unitCubeMatrix ), unitCube, sizeof( unitCube ) );
}

DRRenderer::~DRRenderer()
{
}

bool
DRRenderer::renderFrame( std::list<Instance*>& instances )
{
	if ( false == this->renderShadowMap( instances ) )
		return false;

	if ( false == this->renderGeometryStage( instances ) )
		return false;

//	if ( false == this->renderLightingStage( instances ) )
//		return false;

	for ( int i = 0; i <  MRT_COUNT; i++ )
	{
		if ( false == this->showTexture( this->m_mrt[ i ], i ) )
			return false;
	}

//	if ( false == this->showTexture( this->m_shadowMap, 2 ) )
//		return false;

	// swap buffers
	SDL_GL_SwapBuffers();

	this->frame++;

	return true;
}

bool
DRRenderer::initialize()
{
	cout << "Initializing Deferred Renderer..." << endl;

	glShadeModel( GL_SMOOTH );							// Enable Smooth Shading
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );				// Black Background
	glClearDepth( 1.0f );									// Depth Buffer Setup

	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
	glCullFace(GL_FRONT);

	glEnable( GL_DEPTH_TEST );							// Enables Depth Testing
	glEnable( GL_CULL_FACE );

	if ( false == this->initFBO() )
		return false;

	if ( false == this->initGeomStage() )
		return false;

	if ( false == this->initLightingStage() )
		return false;

	if ( false == this->initShadowMapping() )
		return false;

	if ( false == this->initUniformBlocks() )
		return false;

	cout << "Initializing Deferred Renderer finished" << endl;

	return true;
}

bool
DRRenderer::shutdown()
{
	cout << "Shutting down Deferred Renderer..." << endl;

	// cleaning up uniform blocks
	if ( this->m_mvpTransformBlock )
		delete this->m_mvpTransformBlock;

	if ( this->m_lightDataBlock )
		delete this->m_lightDataBlock;

	if ( this->m_shadowMap )
	{
		glDeleteTextures( 1, &this->m_shadowMap );
	}

	if ( this->m_shadowMappingFB )
	{
		glDeleteFramebuffers( 1, &this->m_shadowMappingFB );
	}

	// cleaning up shadow mapping
	if ( this->m_progShadowMapping )
	{
		if ( this->m_vertShadowMapping )
		{
			this->m_progShadowMapping->detachShader( this->m_vertShadowMapping );

			delete this->m_vertShadowMapping;
			this->m_vertShadowMapping = NULL;
		}

		if ( this->m_fragShadowMapping )
		{
			this->m_progShadowMapping->detachShader( this->m_fragShadowMapping );

			delete this->m_fragShadowMapping;
			this->m_fragShadowMapping = NULL;
		}

		delete this->m_progShadowMapping;
		this->m_progShadowMapping = NULL;
	}

	// cleaning up lighting stage
	if ( this->m_progLightingStage )
	{
		if ( this->m_vertLightingStage )
		{
			this->m_progLightingStage->detachShader( this->m_vertLightingStage );

			delete this->m_vertLightingStage;
			this->m_vertLightingStage = NULL;
		}

		if ( this->m_fragLightingStage )
		{
			this->m_progLightingStage->detachShader( this->m_fragLightingStage );

			delete this->m_fragLightingStage;
			this->m_fragLightingStage = NULL;
		}

		delete this->m_progLightingStage;
		this->m_progLightingStage = NULL;
	}

	// cleaning up geometry-stage
	if ( this->m_progGeomStage )
	{
		if ( this->m_vertLightingStage )
		{
			this->m_progGeomStage->detachShader( this->m_vertLightingStage );

			delete this->m_vertLightingStage;
			this->m_vertLightingStage = NULL;
		}

		if ( this->m_fragGeomStage )
		{
			this->m_progGeomStage->detachShader( this->m_fragGeomStage );

			delete this->m_fragGeomStage;
			this->m_fragGeomStage = NULL;
		}

		delete this->m_progGeomStage;
		this->m_progGeomStage = NULL;
	}

	// cleaning up framebuffer
	if ( this->m_drFB )
	{
		glDeleteFramebuffers( 1, &this->m_drFB );
	}

	// cleaning up mrts
	for ( int i = 0; i < MRT_COUNT; i++ )
	{
		if ( this->m_mrt[ i ] )
		{
			glDeleteTextures( 1, &this->m_mrt[ i ] );
			this->m_mrt[ i ] = 0;
		}
	}

	cout << "Shutting down Deferred Renderer finished" << endl;

	return true;
}

bool
DRRenderer::initFBO()
{
	GLenum status;

	glGenFramebuffers( 1, &this->m_drFB );
	if ( GL_NO_ERROR != ( status = glGetError() )  )
	{
		cout << "ERROR in DRRenderer::initFBO: glGenFramebuffersEXT failed with " << gluErrorString( status ) << " - exit" << endl;
		return false;
	}

	for ( int i = 0; i < MRT_COUNT; i++ )
	{
		this->m_buffers[ i ] = GL_COLOR_ATTACHMENT0 + i;

		glGenTextures( 1, &this->m_mrt[ i ] );
		if ( GL_NO_ERROR != ( status = glGetError() )  )
		{
			cout << "ERROR in DRRenderer::initFBO: glGenTextures failed with " << gluErrorString( status ) << " - exit" << endl;
			return false;
		}

		glBindTexture( GL_TEXTURE_2D, this->m_mrt[ i ] );
		if ( GL_NO_ERROR != ( status = glGetError() )  )
		{
			cout << "ERROR in DRRenderer::initFBO: glBindTexture failed with " << gluErrorString( status ) << " - exit" << endl;
			return false;
		}

		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

		// Remove artifact on the edges of the shadowmap
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );

		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, this->m_camera->getWidth(), this->m_camera->getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, 0 );
		if ( GL_NO_ERROR != ( status = glGetError() )  )
		{
			cout << "ERROR in DRRenderer::initFBO: glTexImage2D failed with " << gluErrorString( status ) << " - exit" << endl;
			return false;
		}

		glBindTexture( GL_TEXTURE_2D, 0 );

		glBindFramebuffer( GL_FRAMEBUFFER, this->m_drFB );
		if ( GL_NO_ERROR != ( status = glGetError() )  )
		{
			cout << "ERROR in DRRenderer::initFBO: glBindFramebuffer failed with " << gluErrorString( status ) << " - exit" << endl;
			return false;
		}

		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, this->m_mrt[ i ], 0 );
		CHECK_FRAMEBUFFER_STATUS( status );
		if ( GL_FRAMEBUFFER_COMPLETE != status )
		{
			cout << "ERROR in DRRenderer::initFBO: framebuffer error: " << gluErrorString( status ) << " - exit" << endl;
			return false;
		}

		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
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
		cout << "ERROR in DRRenderer::initGeomStage: coulnd't create program - exit" << endl;
		return false;
	}

	this->m_vertGeomStage = Shader::createShader( Shader::VERTEX_SHADER, "media/graphics/dr/stages/geom/geomVert.glsl" );
	if ( 0 == this->m_vertGeomStage )
	{
		cout << "ERROR in DRRenderer::initGeomStage: coulnd't create vertex-shader - exit" << endl;
		return false;
	}

	this->m_fragGeomStage = Shader::createShader( Shader::FRAGMENT_SHADER, "media/graphics/dr/stages/geom/geomFrag.glsl" );
	if ( 0 == this->m_fragGeomStage )
	{
		cout << "ERROR in DRRenderer::initGeomStage: coulnd't create fragment-shader - exit" << endl;
		return false;
	}

	if ( false == this->m_vertGeomStage->compile() )
	{
		cout << "ERROR in DRRenderer::initGeomStage: vertex shader compilation failed - exit" << endl;
		return false;
	}

	if ( false == this->m_fragGeomStage->compile() )
	{
		cout << "ERROR in DRRenderer::initGeomStage: fragment shader compilation failed - exit" << endl;
		return false;
	}

	if ( false == this->m_progGeomStage->attachShader( this->m_vertGeomStage ) )
	{
		cout << "ERROR in DRRenderer::initGeomStage: attaching vertex shader to program failed - exit" << endl;
		return false;
	}

	if ( false == this->m_progGeomStage->attachShader( this->m_fragGeomStage ) )
	{
		cout << "ERROR in DRRenderer::initGeomStage: attaching fragment shader to program failed - exit" << endl;
		return false;
	}

	// setting frag-data location is done bevore linking
	if ( false == this->m_progGeomStage->bindFragDataLocation( 0, "out_diffuse" ) )
	{
		cout << "ERROR in DRRenderer::initGeomStage: binding fragment-data location failed - exit" << endl;
		return false;
	}

	if ( false == this->m_progGeomStage->bindFragDataLocation( 1, "out_depth" ) )
	{
		cout << "ERROR in DRRenderer::initGeomStage: binding fragment-data location failed - exit" << endl;
		return false;
	}

	if ( false == this->m_progGeomStage->bindAttribLocation( 0, "in_vertPos" ) )
	{
		cout << "ERROR in DRRenderer::initGeomStage: binding attribute location to program failed - exit" << endl;
		return false;
	}

	if ( false == this->m_progGeomStage->bindAttribLocation( 1, "in_vertNorm" ) )
	{
		cout << "ERROR in DRRenderer::initGeomStage: binding attribute location to program failed - exit" << endl;
		return false;
	}

	if ( false == this->m_progGeomStage->link() )
	{
		cout << "ERROR in DRRenderer::initGeomStage: linking program failed - exit" << endl;
		return false;
	}

	cout << "Initializing Deferred Rendering Geometry-Stage finished" << endl;

	return true;
}

bool
DRRenderer::initLightingStage()
{
	cout << "Initializing Deferred Rendering Lighting-Stage..." << endl;

	this->m_light = new Light( 45.0, 800, 600 );
	this->m_light->setPositionInv( glm::vec3( 0, 80, 120 ) );
	this->m_light->changePitchInv( 30 );

	this->m_progLightingStage = Program::createProgram( );
	if ( 0 == this->m_progLightingStage )
	{
		cout << "ERROR in DRRenderer::initLightingStage: coulnd't create program - exit" << endl;
		return false;
	}

	this->m_vertLightingStage = Shader::createShader( Shader::VERTEX_SHADER, "media/graphics/dr/stages/lighting/lightVert.glsl" );
	if ( 0 == this->m_vertLightingStage )
	{
		cout << "ERROR in DRRenderer::initLightingStage: coulnd't create vertex-shader - exit" << endl;
		return false;
	}

	this->m_fragLightingStage = Shader::createShader( Shader::FRAGMENT_SHADER, "media/graphics/dr/stages/lighting/lightFrag.glsl" );
	if ( 0 == this->m_fragGeomStage )
	{
		cout << "ERROR in DRRenderer::initLightingStage: coulnd't create fragment-shader - exit" << endl;
		return false;
	}

	if ( false == this->m_vertLightingStage->compile() )
	{
		cout << "ERROR in DRRenderer::initLightingStage: vertex shader compilation failed - exit" << endl;
		return false;
	}

	if ( false == this->m_fragLightingStage->compile() )
	{
		cout << "ERROR in DRRenderer::initLightingStage: fragment shader compilation failed - exit" << endl;
		return false;
	}

	if ( false == this->m_progLightingStage->attachShader( this->m_vertLightingStage ) )
	{
		cout << "ERROR in DRRenderer::initLightingStage: attaching vertex shader to program failed - exit" << endl;
		return false;
	}

	if ( false == this->m_progLightingStage->attachShader( this->m_fragLightingStage ) )
	{
		cout << "ERROR in DRRenderer::initLightingStage: attaching fragment shader to program failed - exit" << endl;
		return false;
	}

	if ( false == this->m_progLightingStage->bindAttribLocation( 0, "in_vertPos" ) )
	{
		cout << "ERROR in DRRenderer::initLightingStage: binding attribute location to program failed - exit" << endl;
		return false;
	}

	if ( false == this->m_progLightingStage->link() )
	{
		cout << "ERROR in DRRenderer::initLightingStage: linking program failed - exit" << endl;
		return false;
	}

	cout << "Initializing Deferred Rendering Lighting-Stage finished" << endl;

	return true;
}

bool
DRRenderer::initShadowMapping()
{
	cout << "Initializing Deferred Rendering Shadow-Mapping..." << endl;

	GLenum status;

	// Try to use a texture depth component
	glGenTextures( 1, &this->m_shadowMap );
	if ( GL_NO_ERROR != ( status = glGetError() ) )
	{
		cout << "ERROR in DRRenderer::initShadowMapping: glGenTextures failed with " << gluErrorString( status ) << " - exit" << endl;
		return false;
	}

	glBindTexture( GL_TEXTURE_2D, this->m_shadowMap );
	if ( GL_NO_ERROR != ( status = glGetError() ) )
	{
		cout << "ERROR in DRRenderer::initShadowMapping: glBindTexture failed with " << gluErrorString( status ) << " - exit" << endl;
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

	// No need to force GL_DEPTH_COMPONENT24, drivers usually give you the max precision if available
	glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0 );
	if ( GL_NO_ERROR != ( status = glGetError() ) )
	{
		cout << "ERROR in DRRenderer::initShadowMapping: glTexImage2D failed with " << gluErrorString( status ) << " - exit" << endl;
		return false;
	}

	// unbind framebuffer depth-target
	glBindTexture( GL_TEXTURE_2D, 0 );

	// create a framebuffer object
	glGenFramebuffers( 1, &this->m_shadowMappingFB );
	if ( GL_NO_ERROR != ( status = glGetError() ) )
	{
		cout << "ERROR in DRRenderer::initShadowMapping: glGenFramebuffers failed with " << gluErrorString( status ) << " - exit" << endl;
		return false;
	}

	glBindFramebuffer( GL_FRAMEBUFFER, this->m_shadowMappingFB );
	if ( GL_NO_ERROR != ( status = glGetError() ) )
	{
		cout << "ERROR in DRRenderer::initShadowMapping: glBindFramebuffer failed with " << gluErrorString( status ) << " - exit" << endl;
		return false;
	}

	// Instruct openGL that we won't bind a color texture with the currently binded FBO
	glDrawBuffer( GL_NONE );
	glReadBuffer( GL_NONE );

	// attach the texture to FBO depth attachment point
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D, this->m_shadowMap, 0 );
	CHECK_FRAMEBUFFER_STATUS( status );
	if ( GL_FRAMEBUFFER_COMPLETE != status )
	{
		cout << "ERROR in DRRenderer::initFBO: glFramebufferTexture2D error: " << gluErrorString( status ) << " - exit" << endl;
		return false;
	}

	// switch back to window-system-provided framebuffer
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );

	this->m_progShadowMapping = Program::createProgram( );
	if ( 0 == this->m_progShadowMapping )
	{
		cout << "ERROR in DRRenderer::initShadowMapping: coulnd't create program - exit" << endl;
		return false;
	}

	this->m_vertShadowMapping = Shader::createShader( Shader::VERTEX_SHADER, "media/graphics/dr/stages/geom/shadow/shadowVert.glsl" );
	if ( 0 == this->m_vertShadowMapping )
	{
		cout << "ERROR in DRRenderer::initShadowMapping: coulnd't create vertex shader - exit" << endl;
		return false;
	}

	this->m_fragShadowMapping = Shader::createShader( Shader::FRAGMENT_SHADER, "media/graphics/dr/stages/geom/shadow/shadowFrag.glsl" );
	if ( 0 == this->m_fragShadowMapping )
	{
		cout << "ERROR in DRRenderer::initShadowMapping: coulnd't create fragment shader - exit" << endl;
		return false;
	}

	if ( false == this->m_vertShadowMapping->compile() )
	{
		cout << "ERROR in DRRenderer::initShadowMapping: vertex shader compilation failed - exit" << endl;
		return false;
	}

	if ( false == this->m_fragShadowMapping->compile() )
	{
		cout << "ERROR in DRRenderer::initShadowMapping: fragment shader compilation failed - exit" << endl;
		return false;
	}

	if ( false == this->m_progShadowMapping->attachShader( this->m_vertShadowMapping ) )
	{
		cout << "ERROR in DRRenderer::initShadowMapping: attaching vertex shader to program failed - exit" << endl;
		return false;
	}

	if ( false == this->m_progShadowMapping->attachShader( this->m_fragShadowMapping ) )
	{
		cout << "ERROR in DRRenderer::initShadowMapping: attaching fragment shader to program failed - exit" << endl;
		return false;
	}

	if ( false == this->m_progShadowMapping->bindAttribLocation( 0, "in_vertPos" ) )
	{
		cout << "ERROR in DRRenderer::initShadowMapping: binding attribute location to program failed - exit" << endl;
		return false;
	}

	if ( false == this->m_progShadowMapping->link() )
	{
		cout << "ERROR in DRRenderer::initShadowMapping: linking program failed - exit" << endl;
		return false;
	}

	cout << "Initializing Deferred Rendering Shadow-Mapping finished" << endl;

	return true;
}

bool
DRRenderer::initUniformBlocks()
{
	this->m_mvpTransformBlock = UniformBlock::createBlock( "mvp_transform" );
	if ( 0 == this->m_mvpTransformBlock )
	{
		cout << "ERROR in DRRenderer::initUniformBlocks: creating uniform block failed - exit" << endl;
		return false;
	}

	this->m_lightDataBlock = UniformBlock::createBlock( "lightData" );
	if ( 0 == this->m_lightDataBlock )
	{
		cout << "ERROR in DRRenderer::initUniformBlocks: creating uniform block failed - exit" << endl;
		return false;
	}

	// bind uniform blocks
	if ( false == this->m_mvpTransformBlock->bind() )
	{
		cout << "ERROR in DRRenderer::initUniformBlocks: binding transform uniform-block failed - exit" << endl;
		return false;
	}
	if ( false == this->m_lightDataBlock->bind() )
	{
		cout << "ERROR in DRRenderer::initUniformBlocks: binding transform uniform-block failed - exit" << endl;
		return false;
	}

	// bind mvp-transformation to all programs
	if ( false == this->m_progShadowMapping->bindUniformBlock( this->m_mvpTransformBlock ) )
	{
		cout << "ERROR in DRRenderer::initUniformBlocks: failed binding uniform block - exit" << endl;
		return false;
	}
	if ( false == this->m_progGeomStage->bindUniformBlock( this->m_mvpTransformBlock ) )
	{
		cout << "ERROR in DRRenderer::initUniformBlocks: failed binding uniform block - exit" << endl;
		return false;
	}
	if ( false == this->m_progLightingStage->bindUniformBlock( this->m_mvpTransformBlock ) )
	{
		cout << "ERROR in DRRenderer::initUniformBlocks: failed binding uniform block - exit" << endl;
		return false;
	}

	// lighting data just to lighting stage program
	if ( false == this->m_progGeomStage->bindUniformBlock( this->m_lightDataBlock ) )
	{
		cout << "ERROR in DRRenderer::initUniformBlocks: failed binding uniform block - exit" << endl;
		return false;
	}
	if ( false == this->m_progLightingStage->bindUniformBlock( this->m_lightDataBlock ) )
	{
		cout << "ERROR in DRRenderer::initUniformBlocks: failed binding uniform block - exit" << endl;
		return false;
	}

	return true;
}

bool
DRRenderer::renderShadowMap( std::list<Instance*>& instances )
{
	GLenum status;

	if ( false == this->m_progShadowMapping->use() )
		return false;

	// Rendering offscreen
	glBindFramebuffer( GL_FRAMEBUFFER, this->m_shadowMappingFB );
	if ( GL_NO_ERROR != ( status = glGetError() ) )
	{
		cout << "ERROR in DRRenderer::renderShadowMap: glBindFramebuffer failed with " << gluErrorString( status ) << endl;
		return false;
	}

	CHECK_FRAMEBUFFER_STATUS( status );
	if ( GL_FRAMEBUFFER_COMPLETE != status )
	{
		cout << "ERROR in DRRenderer::renderGeometryStage: framebuffer error: " << gluErrorString( status ) << " - exit" << endl;
		return false;
	}

	// cull front-faces, just backfaces cast a shadow -> better quality
	//glCullFace( GL_FRONT );
	glClear( GL_DEPTH_BUFFER_BIT );
	glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );

	if ( false == this->renderInstances( this->m_light, instances ) )
		return false;

	// back to window-system framebuffer
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	if ( GL_NO_ERROR != ( status = glGetError() ) )
	{
		cout << "ERROR in DRRenderer::renderShadowMap: glBindFramebuffer( 0 ) failed with " << gluErrorString( status ) << endl;
		return false;
	}

	return true;
}

bool
DRRenderer::renderGeometryStage( std::list<Instance*>& instances )
{
	GLenum status;

	if ( false == this->m_progGeomStage->use() )
	{
		cout << "ERROR in DRRenderer::renderGeometryStage: using shadow mapping program failed - exit" << endl;
		return false;
	}

	// start geometry pass
	glBindFramebuffer( GL_FRAMEBUFFER, this->m_drFB );
	if ( GL_NO_ERROR != ( status = glGetError() ) )
	{
		cout << "ERROR in DRRenderer::renderGeometryStage: glBindFramebuffer failed with " << gluErrorString( status ) << endl;
		return false;
	}

	// activate drawing to targets
	glDrawBuffers( MRT_COUNT, this->m_buffers );
	if ( GL_NO_ERROR != ( status = glGetError() ) )
	{
		cout << "ERROR in DRRenderer::renderGeometryStage: glDrawBuffers failed with " << gluErrorString( status ) << endl;
		return false;
	}

	CHECK_FRAMEBUFFER_STATUS( status );
	if ( GL_FRAMEBUFFER_COMPLETE != status )
	{
		cout << "ERROR in DRRenderer::renderGeometryStage: framebuffer error: " << gluErrorString( status ) << " - exit" << endl;
		return false;
	}

	glm::mat4 lightSpace = this->m_unitCubeMatrix * this->m_light->m_PVMatrix;
	// update the transform-uniforms block with the new mvp matrix
	if ( false == this->m_lightDataBlock->updateData( glm::value_ptr( lightSpace ), 0, 64) )
		return false;
	// update the inverse projection ( could also be carried out on the GPU but we calculate it once on the cpu )
	if ( false == this->m_mvpTransformBlock->updateData( glm::value_ptr( lightSpace ), 64, 64) )
		return false;

	// bind the shadowmap of the global light to texture-unit 0
	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, this->m_shadowMap );
	if ( GL_NO_ERROR != ( status = glGetError() ) )
	{
		cout << "ERROR in DRRenderer::renderLightingStage: glBindTexture failed with " << gluErrorString( status ) << endl;
		return false;
	}

	// tell program that the uniform sampler2D called ShadowMap points now to texture-unit MRT_COUNT
	if ( false == this->m_progGeomStage->setUniformInt( "ShadowMap", 0 ) )
		return false;

	// switch to back-face culling
	glCullFace( GL_BACK );
	glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
	glClearColor( 1.0, 0.0, 0.0, 1.0 );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// draw all geometry
	if ( false == this->renderInstances( this->m_camera, instances ) )
		return false;

	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	if ( GL_NO_ERROR != ( status = glGetError() ) )
	{
		cout << "ERROR in DRRenderer::renderGeometryStage: glBindFramebuffer( 0 ) failed with " << gluErrorString( status ) << endl;
		return false;
	}

	return true;
}

bool
DRRenderer::renderLightingStage( std::list<Instance*>& instances )
{
	GLenum status;

	// activate lighting-stage shader
	if ( false == this->m_progLightingStage->use() )
		return false;


	// bind diffuse rendering target to texture unit 0
	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, this->m_mrt[ 0 ] );
	if ( GL_NO_ERROR != ( status = glGetError() ) )
	{
		cout << "ERROR in DRRenderer::renderLightingStage: glBindTexture of mrt 0 failed with " << gluErrorString( status ) << endl;
		return false;
	}
	// tell lighting program that diffusemap is boudn to texture-unit 0
	if ( false == this->m_progLightingStage->setUniformInt( "DiffuseMap", 0 ) )
		return false;


	// bind depth rendering target to texture unit 1
	glActiveTexture( GL_TEXTURE1 );
	glBindTexture( GL_TEXTURE_2D, this->m_mrt[ 1 ] );
	if ( GL_NO_ERROR != ( status = glGetError() ) )
	{
		cout << "ERROR in DRRenderer::renderLightingStage: glBindTexture mrt 1 failed with " << gluErrorString( status ) << endl;
		return false;
	}
	// tell lighting program that depthmap is boudn to texture-unit 1
	if ( false == this->m_progLightingStage->setUniformInt( "DepthMap", 1 ) )
		return false;


	// bind the shadowmap of the global light to texture-unit 2
	glActiveTexture( GL_TEXTURE2 );
	glBindTexture( GL_TEXTURE_2D, this->m_shadowMap );
	if ( GL_NO_ERROR != ( status = glGetError() ) )
	{
		cout << "ERROR in DRRenderer::renderLightingStage: glBindTexture failed with " << gluErrorString( status ) << endl;
		return false;
	}
	// tell program that the uniform sampler2D called ShadowMap points now to texture-unit MRT_COUNT
	if ( false == this->m_progLightingStage->setUniformInt( "ShadowMap", 2 ) )
		return false;


	glm::mat4 inverseProjection = glm::inverse( this->m_camera->m_projectionMatrix );
	// calculate the light-space projection matrix
	// multiplication with unit-cube is first because has to be carried out the last
	glm::mat4 lightSpace = this->m_unitCubeMatrix * this->m_light->m_PVMatrix;
	// update the transform-uniforms block with the new mvp matrix
	if ( false == this->m_lightDataBlock->updateData( glm::value_ptr( lightSpace ), 0, 64) )
		return false;


	this->m_camera->setupOrtho();

	// update projection-view because changed to orthogonal-projection
	if ( false == this->m_mvpTransformBlock->updateData( glm::value_ptr( this->m_camera->m_projectionMatrix ), 0, 64) )
		return false;
	// update the inverse projection ( could also be carried out on the GPU but we calculate it once on the cpu )
	if ( false == this->m_mvpTransformBlock->updateData( glm::value_ptr( inverseProjection ), 64, 64) )
		return false;

	// render quad
	glBegin( GL_QUADS );
		glVertex2f( 0, 0 );
		glVertex2f( 0, this->m_camera->getHeight() );
		glVertex2f( this->m_camera->getWidth(), this->m_camera->getHeight() );
		glVertex2f( this->m_camera->getWidth(), 0 );
	glEnd();

	this->m_camera->setupPerspective();


	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, 0 );

	glActiveTexture( GL_TEXTURE1 );
	glBindTexture( GL_TEXTURE_2D, 0 );

	glActiveTexture( GL_TEXTURE2 );
	glBindTexture( GL_TEXTURE_2D, 0 );

	return true;
}

bool
DRRenderer::renderInstances( Viewer* viewer, std::list<Instance*>& instances )
{
	list<Instance*>::iterator iter = instances.begin();
	while ( iter != instances.end() )
	{
		Instance* instance = *iter++;

		if ( false == this->renderGeom( viewer, instance, instance->geom ) )
			return false;
	}

	return true;
}

bool
DRRenderer::renderGeom( Viewer* viewer, Instance* parent, GeomType* geom )
{
	if ( geom->children.size() )
	{
		for ( unsigned int i = 0; i < geom->children.size(); i++ )
		{
			if ( false == this->renderGeom( viewer, parent, geom->children[ i ] ) )
				return false;
		}
	}
	else
	{
		Viewer::CullResult cullResult = viewer->cullBB( geom->getBBMin(), geom->getBBMax() );
		if ( Viewer::OUTSIDE != cullResult )
		{
			glm::mat4 mvp = viewer->m_PVMatrix * *parent->m_modelMatrix * geom->m_modelMatrix;
			if ( false == this->m_mvpTransformBlock->updateData( glm::value_ptr( mvp ), 0, 64) )
				return false;

			// Don't need to update inverse, because its just needed in screen-space during lighting-stage

			return geom->render();
		}
	}

	return true;
}

bool
DRRenderer::showTexture( GLuint texID, int quarter )
{
	GLint status;

	glUseProgram( 0 );
	if ( GL_NO_ERROR != ( status = glGetError() ) )
	{
		cout << "ERROR in DRRenderer::showTexture: glUseProgram( 0 ) failed with " << gluErrorString( status ) << endl;
		return false;
	}

	// set up orthogonal projection to render quad
	this->m_camera->setupOrtho();

	glEnable( GL_TEXTURE_2D );
	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, texID );
	if ( GL_NO_ERROR != ( status = glGetError() ) )
	{
		cout << "ERROR in DRRenderer::showTexture: glBindTexture failed with " << gluErrorString( status ) << endl;
		return false;
	}

	int counter = 0;
	float height = this->m_camera->getHeight() / 2;
	float width = this->m_camera->getWidth() / 2;

	for ( int i = 0; i < 2; i++ )
	{
		for ( int j = 0; j < 2; j++ )
		{
			if ( counter == quarter )
			{
				// render quad
				glBegin( GL_QUADS );
					glTexCoord2f( 0.0f, 1.0f ); glVertex2f( width * j, height * i );
					glTexCoord2f( 0.0f, 0.0f ); glVertex2f( width * j, height * i + height );
					glTexCoord2f( 1.0f, 0.0f ); glVertex2f( width * j + width, height * i + height );
					glTexCoord2f( 1.0f, 1.0f ); glVertex2f( width * j + width, height * i );
				glEnd();
			}

			counter++;
		}
	}

	glBindTexture( GL_TEXTURE_2D, 0 );
	glDisable( GL_TEXTURE_2D );

	// switch back to perspective projection
	this->m_camera->setupPerspective();

	return true;
}
