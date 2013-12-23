#include "RenderTarget.h"

#include "../ZazenGraphics.h"
#include "../Util/GLUtils.h"

#include <iostream>

using namespace std;

vector<RenderTarget*>
RenderTarget::m_allTargets;

RenderTarget*
RenderTarget::create( GLsizei width, GLsizei height, RenderTargetType targetType )
{
	GLuint id = 0;

	// use shadow-map pooling: only create maps with different resolution and reuse them 
	// this is possible only through the use of a deferred-lighting aproach where lighting is decoupled
	// from geometry-rendering so each light can be rendererd one after another so only need
	// to hold the current shadow-map in memory
	if ( RenderTarget::RT_SHADOW_PLANAR == targetType || RT_SHADOW_CUBE == targetType )
	{
		RenderTarget* pooledShadowMap = RenderTarget::findShadowMapInPool( targetType, width, height );
		if ( NULL != pooledShadowMap )
		{
			return pooledShadowMap;
		}
	}

	glGenTextures( 1, &id );
	if ( GL_PEEK_ERRORS )
	{
		ZazenGraphics::getInstance().getLogger().logError() << "RenderTarget::create: glGenTextures failed - exit";
		return NULL;
	}

	RenderTarget* renderTarget = NULL;

	if ( RenderTarget::RT_COLOR == targetType || RenderTarget::RT_DEPTH == targetType || RenderTarget::RT_DEPTH_STENCIL == targetType )
	{
		glBindTexture( GL_TEXTURE_2D, id );
		GL_PEEK_ERRORS_AT

		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		GL_PEEK_ERRORS_AT
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		GL_PEEK_ERRORS_AT
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		GL_PEEK_ERRORS_AT
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		GL_PEEK_ERRORS_AT
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0 ); 
		GL_PEEK_ERRORS_AT
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0 ); 
		GL_PEEK_ERRORS_AT

		if ( RenderTarget::RT_COLOR == targetType )
		{
			glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL );
			GL_PEEK_ERRORS_AT
		}
		else if ( RenderTarget::RT_DEPTH == targetType )
		{
			glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL );
			GL_PEEK_ERRORS_AT
		}
		else
		{
			glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL );
			GL_PEEK_ERRORS_AT
		}

		renderTarget = new RenderTarget( id, width, height, targetType, Texture::TEXTURE_2D );
	}
	else if ( RenderTarget::RT_SHADOW_PLANAR == targetType )
	{
		glBindTexture( GL_TEXTURE_2D, id );
		GL_PEEK_ERRORS_AT

		// need linear filtering for shadow-maps
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		GL_PEEK_ERRORS_AT
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		GL_PEEK_ERRORS_AT
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		GL_PEEK_ERRORS_AT
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		GL_PEEK_ERRORS_AT
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0 ); 
		GL_PEEK_ERRORS_AT
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0 ); 
		GL_PEEK_ERRORS_AT

		// need to enable comparison-mode for depth-texture to use it as a textureProj/texture in shader		
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL );
		GL_PEEK_ERRORS_AT
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE );
		GL_PEEK_ERRORS_AT

		glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL );
		GL_PEEK_ERRORS_AT

		renderTarget = new RenderTarget( id, width, height, targetType, Texture::TEXTURE_2D );
	}
	else if ( RenderTarget::RT_SHADOW_CUBE == targetType )
	{
		glBindTexture( GL_TEXTURE_CUBE_MAP, id );
		GL_PEEK_ERRORS_AT

		// need linear filtering for shadow-maps
		glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		GL_PEEK_ERRORS_AT
		glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR ); 
		GL_PEEK_ERRORS_AT
		glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		GL_PEEK_ERRORS_AT
		glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		GL_PEEK_ERRORS_AT
		glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );
		GL_PEEK_ERRORS_AT
		glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0 ); 
		GL_PEEK_ERRORS_AT
		glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0 ); 
		GL_PEEK_ERRORS_AT

		// NOTE: we don't do hardware-comparison like GL_COMPARE_R_TO_TEXTURE
		// we store radial distance from light and then compare in shader
		/*	
		glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL );
		GL_PEEK_ERRORS_AT
		glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE );
		GL_PEEK_ERRORS_AT
		*/

		for ( int i = 0; i < 6; i++ )
		{
			glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL );
			GL_PEEK_ERRORS_AT
		}

		renderTarget = new RenderTarget( id, width, height, targetType, Texture::TEXTURE_CUBE );
	}
	else if ( RenderTarget::RT_COLOR_CUBE == targetType )
	{
		glBindTexture( GL_TEXTURE_CUBE_MAP, id );
		GL_PEEK_ERRORS_AT

		glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
		GL_PEEK_ERRORS_AT
		glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		GL_PEEK_ERRORS_AT
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		GL_PEEK_ERRORS_AT
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		GL_PEEK_ERRORS_AT
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		GL_PEEK_ERRORS_AT
		glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0 ); 
		GL_PEEK_ERRORS_AT
		glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 10 ); 
		GL_PEEK_ERRORS_AT
		glTexParameterf( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4.0f ); 
		GL_PEEK_ERRORS_AT
		
		for ( int i = 0; i < 6; i++ )
		{
			glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
			GL_PEEK_ERRORS_AT
		}

		renderTarget = new RenderTarget( id, width, height, targetType, Texture::TEXTURE_CUBE );
	}
	else
	{
		ZazenGraphics::getInstance().getLogger().logError( "RenderTarget::create: unsupported RenderTarget-Type - exit" );
		return NULL;
	}

	RenderTarget::m_allTargets.push_back( renderTarget );

	return renderTarget;
}

RenderTarget*
RenderTarget::findShadowMapInPool( RenderTargetType targetType, GLsizei width, GLsizei height )
{
	// NOTE: only shadow&depth targets can be shared

	for ( unsigned int i = 0; i < RenderTarget::m_allTargets.size(); i++ )
	{
		RenderTarget* shadowMap = RenderTarget::m_allTargets[ i ];
		if ( targetType == shadowMap->getType() )
		{
			if ( shadowMap->getWidth() == width && shadowMap->getHeight() == height )
			{
				return shadowMap;
			}
		}
	}

	return NULL;
}

bool
RenderTarget::destroy( RenderTarget* renderTarget )
{
	if ( NULL == renderTarget )
	{
		return true;
	}

	delete renderTarget;

	return true;
}

void
RenderTarget::cleanup()
{
	for ( unsigned int i = 0; i < RenderTarget::m_allTargets.size(); i++ )
	{
		RenderTarget* shadowMap = RenderTarget::m_allTargets[ i ];
		RenderTarget::destroy( shadowMap );
	}

	RenderTarget::m_allTargets.clear();
}

RenderTarget::RenderTarget( GLuint id, GLsizei width, GLsizei height, RenderTargetType targetType, TextureType textureType )
	: Texture( id, textureType )
{
	this->m_targetType = targetType;

	this->m_width = width;
	this->m_height = height;
}

RenderTarget::~RenderTarget()
{
}
