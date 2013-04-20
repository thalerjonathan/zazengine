/*
 *  texture.cpp
 *  ZENgine
 *
 *  Created by Jonathan Thaler on 30.04.08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "Texture.h"

#include "../ZazenGraphics.h"
#include "../util/GLUtils.h"

#include <iostream>

using namespace std;

GLint Texture::m_currentTextureUnit = -1;

Texture::Texture( GLuint texID, TextureType type )
{
	this->m_id = texID;
	this->m_textureType = type;
}

Texture::~Texture()
{
	glDeleteTextures( 1, &this->m_id );
}

bool
Texture::bind( int textureUnit )
{
	if ( Texture::m_currentTextureUnit != textureUnit )
	{
		glActiveTexture( GL_TEXTURE0 + textureUnit );

#ifdef CHECK_GL_ERRORS
		if ( false == GLUtils::peekErrors() )
		{
			ZazenGraphics::getInstance().getLogger().logError() << "Zexture::bind: failed glActiveTexture";
			return false;
		}
#endif

		Texture::m_currentTextureUnit = textureUnit;
	}

	if ( Texture::TEXTURE_2D == this->m_textureType ) 
	{
		glBindTexture( GL_TEXTURE_2D, this->m_id );

#ifdef CHECK_GL_ERRORS
		if ( false == GLUtils::peekErrors() )
		{
			ZazenGraphics::getInstance().getLogger().logError() << "Texture::bind: failed glBindTexture";
			return false;
		}
#endif

	}
	else if ( Texture::TEXTURE_CUBE == this->m_textureType ) 
	{
		glBindTexture( GL_TEXTURE_CUBE_MAP, this->m_id );

#ifdef CHECK_GL_ERRORS
		if ( false == GLUtils::peekErrors() )
		{
			ZazenGraphics::getInstance().getLogger().logError() << "Texture::bind: failed glBindTexture";
			return false;
		}
#endif
	}

	return true;
}
