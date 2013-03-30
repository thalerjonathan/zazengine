/*
 * Light.cpp
 *
 *  Created on: Feb 4, 2011
 *      Author: jonathan
 */

#include "Light.h"

#include <string.h>
#include <iostream>

using namespace std;

Light*
Light::createSpotLight( float fov, int width, int height, bool shadowCaster )
{
	Light* light = new Light( width, height, Light::SPOT, shadowCaster );
	if ( false == light->createShadowMap( width, height ) )
	{
		delete light;
		return NULL;
	}
	
	light->setFov( fov );
	light->setupPerspective();

	return light;
}

Light*
Light::createDirectionalLight( int width, int height, bool shadowCaster )
{
	Light* light = new Light( width, height, Light::DIRECTIONAL, shadowCaster );
	if ( false == light->createShadowMap( width, height ) )
	{
		delete light;
		return NULL;
	}
	
	light->setupOrtho();

	return light;
}

Light*
Light::createPointLight( int side, bool shadowCaster )
{
	Light* light = new Light( side, side, Light::POINT, shadowCaster );
	if ( false == light->createShadowCubeMap( side ) )
	{
		delete light;
		return NULL;
	}

	light->setFov( 90 );
	light->setupPerspective();

	return light;
}

Light::Light( int width, int height, LightType type, bool shadowCaster )
	: Viewer( width, height ),
	  m_type( type ),
	  m_shadowCaster( shadowCaster )
{
	this->m_shadowMap = 0;
	memset( this->m_cubeShadowMap, 0, sizeof( this->m_cubeShadowMap ) );

	this->m_falloff = 0.0f;

	this->m_shadowMap = NULL;
}

Light::~Light()
{
	RenderTarget::destroy( this->m_shadowMap );

	//TODO handle different when implemented
	if ( this->m_cubeShadowMap[ 0 ] )
	{
		glDeleteTextures( 6, this->m_cubeShadowMap );
	}
}

bool
Light::createShadowMap( int width, int height )
{
	if ( this->m_shadowCaster )
	{
		RenderTarget* shadowMap = RenderTarget::create( width, height, RenderTarget::RT_SHADOW );
		if ( NULL == shadowMap )
		{
			return false;
		}

		this->m_shadowMap = shadowMap;
	}

	return true;
}

bool
Light::createShadowCubeMap( int side )
{
	// TODO implement
	return true;
}
