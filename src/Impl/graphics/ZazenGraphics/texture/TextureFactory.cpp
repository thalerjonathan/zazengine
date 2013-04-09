/*
 *  texture.cpp
 *  ZENgine
 *
 *  Created by Jonathan Thaler on 30.04.08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "TextureFactory.h"

#include <iostream>

using namespace std;
using namespace boost;

map<string, Texture*> TextureFactory::allTextures;
boost::filesystem::path TextureFactory::textureDataPath;

void
TextureFactory::init( const boost::filesystem::path& textureDataPath )
{
	ilInit();
	iluInit();
	ilutInit();

	TextureFactory::textureDataPath = textureDataPath;
}

Texture*
TextureFactory::get( const std::string& file )
{
	map<string, Texture*>::iterator findIter = TextureFactory::allTextures.find( file );
	if ( findIter != TextureFactory::allTextures.end() )
	{
		return findIter->second;
	}

	filesystem::path fullFileName( TextureFactory::textureDataPath.generic_string() + file );

	if ( false == filesystem::exists( fullFileName ) )
	{
		cout << "ERROR ... in Texture::get: file for texture " << fullFileName << " does not exist" << endl;
		return NULL;
	}

	if ( filesystem::is_directory( fullFileName ) )
	{
		cout << "ERROR ... in Texture::get: file for texture " << fullFileName << " is a directory" << endl;
		return NULL;
	}

	ILuint imageId;
	if ( false == TextureFactory::createImage( fullFileName.generic_string(), &imageId ) )
	{
		return NULL;
	}

	GLuint textureID = TextureFactory::createTexture( imageId );
	if ( 0 == textureID )
	{
		return NULL;
	}

	cout << "INFO ... successfully loaded texture from " << fullFileName << endl;

	Texture* newTexture = new Texture( textureID, Texture::TEXTURE_2D );
	
	TextureFactory::allTextures[ file ] = newTexture;
	
	return newTexture;
}

Texture*
TextureFactory::getCube( const boost::filesystem::path& cubeMapPath, const std::string& fileType )
{
	map<string, Texture*>::iterator findIter = TextureFactory::allTextures.find( cubeMapPath.generic_string() );
	if ( findIter != TextureFactory::allTextures.end() )
	{
		return findIter->second;
	}

	filesystem::path fullFileName( TextureFactory::textureDataPath.generic_string() + cubeMapPath.generic_string() );

	if ( false == filesystem::exists( fullFileName ) )
	{
		cout << "ERROR ... in Texture::getCube: file for texture " << fullFileName << " does not exist" << endl;
		return NULL;
	}

	if ( false == filesystem::is_directory( fullFileName ) )
	{
		cout << "ERROR ... in Texture::getCube: file for texture " << fullFileName << " is NOT a directory" << endl;
		return NULL;
	}

	ILuint* imageIds = NULL;

	vector<string> fileNames;
	fileNames.push_back( fullFileName.generic_string() + "/xpos" + "." + fileType );
	fileNames.push_back( fullFileName.generic_string() + "/xneg" + "." + fileType );
	fileNames.push_back( fullFileName.generic_string() + "/ypos" + "." + fileType );
	fileNames.push_back( fullFileName.generic_string() + "/yneg" + "." + fileType );
	fileNames.push_back( fullFileName.generic_string() + "/zpos" + "." + fileType );
	fileNames.push_back( fullFileName.generic_string() + "/zneg" + "." + fileType );

	if ( false == TextureFactory::createImages( fileNames, &imageIds ) )
	{
		return NULL;
	}

	GLuint textureID = TextureFactory::createCubeTexture( imageIds );
	if ( 0 == textureID )
	{
		return NULL;
	}

	cout << "INFO ... successfully loaded texture from " << fullFileName << endl;

	Texture* newTexture = new Texture( textureID, Texture::TEXTURE_CUBE );
	
	TextureFactory::allTextures[ cubeMapPath.generic_string() ] = newTexture;
	
	return newTexture;
}

void
TextureFactory::freeAll()
{
	map<string, Texture*>::iterator iter = TextureFactory::allTextures.begin();
	while(iter != TextureFactory::allTextures.end())
	{
		delete iter->second;
		
		iter++;
	}
	
	TextureFactory::allTextures.clear();
}

bool
TextureFactory::createImage( const std::string& fileName, ILuint* imageId )
{
	ilGenImages( 1, imageId );

	if ( false == TextureFactory::loadImage( fileName, *imageId ) )
	{
		ilDeleteImages( 1, imageId );
		return false;
	}

	return true;
}

bool
TextureFactory::createImages( const std::vector<std::string>& fileNames, ILuint** imageIds )
{
	bool errorFlag = false;
	*imageIds = ( ILuint* ) malloc( fileNames.size() * sizeof( ILuint ) );
	memset( *imageIds, 0, fileNames.size() * sizeof( ILuint ) );

	ilGenImages( fileNames.size(), *imageIds );

	for ( unsigned int i = 0; i < fileNames.size(); i++ )
	{
		if ( false == TextureFactory::loadImage( fileNames[ i ], (*imageIds)[ i ] ) )
		{
			errorFlag = true;
			break;
		}
	}

	if ( errorFlag )
	{
		ilDeleteImages( fileNames.size(), *imageIds );
		free( *imageIds );
		return false;
	}

	return true;
}

bool
TextureFactory::loadImage( const std::string& fileName, ILuint imageId )
{
	ilBindImage( imageId );

	ILboolean success = ilLoadImage( fileName.c_str() );
	if ( success )
	{
		// If the image is flipped (i.e. upside-down and mirrored, flip it the right way up!)
		ILinfo ImageInfo;
		iluGetImageInfo( &ImageInfo );
		if ( IL_ORIGIN_UPPER_LEFT == ImageInfo.Origin )
		{
			iluFlipImage();
		}

		// ... then attempt to conver it.
		// NOTE: If your image contains alpha channel you can replace IL_RGB with IL_RGBA
		success = ilConvertImage( IL_RGB, IL_UNSIGNED_BYTE );

		// Quit out if we failed the conversion
		if ( !success )
		{
			cout << "ERROR ... in Texture::loadImage: Image load failed - IL reports error: " << ilGetError() << endl;
			return false;
		}
 	}
  	else // If we failed to open the image file in the first place...
  	{
		cout << "ERROR ... in Texture::loadImage: Image load failed - IL reports error: " << ilGetError() << endl;
		return false;
  	}

	return true;
}

// TODO: error handling glGetError!
GLuint
TextureFactory::createTexture( ILuint imageId )
{
	GLuint textureId;			// Create a texture ID as a GLuint

	ilBindImage( imageId ); 			// Bind the image

	// Generate a new texture
	glGenTextures( 1, &textureId );

	// Bind the texture to a name
	glBindTexture( GL_TEXTURE_2D, textureId );

	// Set texture clamping method
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );

	// Set texture interpolation method to use linear interpolation (no MIPMAPS)
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		
	// Specify the texture specification
	glTexImage2D(GL_TEXTURE_2D, 				// Type of texture
					0,				// Pyramid level (for mip-mapping) - 0 is the top level
					ilGetInteger( IL_IMAGE_BPP ),	// Image colour depth
					ilGetInteger( IL_IMAGE_WIDTH ),	// Image width
					ilGetInteger( IL_IMAGE_HEIGHT ),	// Image height
					0,				// Border width in pixels (can either be 1 or 0)
					ilGetInteger( IL_IMAGE_FORMAT ),	// Image format (i.e. RGB, RGBA, BGR etc.) GL_RGB
					GL_UNSIGNED_BYTE,		// Image data type
					ilGetData() );			// The actual image data itself

	// deprecated: no more necessary
	glBindTexture( GL_TEXTURE_2D, 0 );

 	ilDeleteImages( 1, &imageId ); // Because we have already copied image data into texture data we can release memory used by image.

	return textureId; // Return the GLuint to the texture so you can use it!
}

// TODO: error handling glGetError!
GLuint
TextureFactory::createCubeTexture( ILuint* imageIds )
{
	GLuint textureId;

	glEnable( GL_TEXTURE_CUBE_MAP );
	glGenTextures( 1, &textureId );
	glBindTexture( GL_TEXTURE_CUBE_MAP, textureId );
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST ); 
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );

	ilBindImage( imageIds[ 0 ] );
	glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, ilGetInteger( IL_IMAGE_WIDTH ), 
		ilGetInteger( IL_IMAGE_HEIGHT ), 0, GL_RGB, GL_UNSIGNED_BYTE, ilGetData() );

	ilBindImage( imageIds[ 1 ] );
	glTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, ilGetInteger( IL_IMAGE_WIDTH ), 
		ilGetInteger( IL_IMAGE_HEIGHT ), 0, GL_RGB, GL_UNSIGNED_BYTE, ilGetData() );

	ilBindImage( imageIds[ 2 ] );
	glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, ilGetInteger( IL_IMAGE_WIDTH ), 
		ilGetInteger( IL_IMAGE_HEIGHT ), 0, GL_RGB, GL_UNSIGNED_BYTE, ilGetData() );

	ilBindImage( imageIds[ 3 ] );
	glTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, ilGetInteger( IL_IMAGE_WIDTH ), 
		ilGetInteger( IL_IMAGE_HEIGHT ), 0, GL_RGB, GL_UNSIGNED_BYTE, ilGetData() );

	ilBindImage( imageIds[ 4 ] );
	glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, ilGetInteger( IL_IMAGE_WIDTH ), 
		ilGetInteger( IL_IMAGE_HEIGHT ), 0, GL_RGB, GL_UNSIGNED_BYTE, ilGetData() );

	ilBindImage( imageIds[ 5 ] );
	glTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, ilGetInteger( IL_IMAGE_WIDTH ), 
		ilGetInteger( IL_IMAGE_HEIGHT ), 0, GL_RGB, GL_UNSIGNED_BYTE, ilGetData() );

	return textureId;
}
