/*
 * Shader.cpp
 *
 *  Created on: Jan 23, 2011
 *      Author: jonathan
 */

#include "Shader.h"

#include <stdlib.h>
#include <stdio.h>

#include <iostream>

using namespace std;

Shader*
Shader::createShader( Shader::ShaderType type, const std::string& file )
{
	GLuint shaderObject = 0;
	GLint status;
	std::string source;
	const char* sourcePtr = NULL;

	if (Shader::readShaderSource( file, source ) == false)
		return 0;

	if ( Shader::VERTEX_SHADER == type )
	{
		shaderObject = glCreateShader( GL_VERTEX_SHADER );
		if (0 == shaderObject )
		{
			cout << "glCreateShader for GL_VERTEX_SHADER \"" << file << "\" failed with " << gluErrorString(glGetError()) << endl;
			return 0;
		}
	}
	else if ( Shader::FRAGMENT_SHADER == type )
	{
		shaderObject = glCreateShader( GL_FRAGMENT_SHADER );
		if ( 0 == shaderObject )
		{
			cout << "glCreateShader for GL_FRAGMENT_SHADER \"" << file << "\" failed with " << gluErrorString(glGetError()) << endl;
			return 0;
		}
	}
	else if ( Shader::GEOMETRY_SHADER == type )
	{
		shaderObject = glCreateShader( GL_GEOMETRY_SHADER_EXT );
		if ( 0 == shaderObject )
		{
			cout << "glCreateShader for GL_GEOMETRY_SHADER_EXT \"" << file << "\" failed with " << gluErrorString(glGetError()) << endl;
			return 0;
		}
	}

	sourcePtr = source.c_str();
	glShaderSource( shaderObject, 1, (const GLchar**) &sourcePtr, NULL);
	status = glGetError();
	if ( GL_NO_ERROR != status )
	{
		cout << "glShaderSource for \"" << file << "\" failed with " << gluErrorString( status ) << endl;
		return 0;
	}


	return new Shader( shaderObject );
}

Shader::Shader( GLuint shaderObject )
{
	this->shaderObject = shaderObject;
}

Shader::~Shader()
{
}

bool
Shader::compile()
{
	GLint status;

	glCompileShader( this->shaderObject );
	glGetShaderiv( this->shaderObject, GL_COMPILE_STATUS, &status );
	if ( GL_TRUE != status )
	{
		cout << "Failed compiling" << endl;
		Shader::printInfoLog( this->shaderObject );
		return false;
	}

	return true;
}

bool
Shader::readShaderSource(const string& file, string& shaderSource)
{
	string fullFileName = file;

	FILE* shaderSourceFile = fopen(fullFileName.c_str(), "r");
	if (shaderSourceFile == 0) {
		cout << "ERROR ... couldn't open Shadersource-File " << fullFileName << endl;
		return false;
	}

	char c;
	while ((c = fgetc(shaderSourceFile)) != EOF)
		shaderSource += c;

	fclose( shaderSourceFile );

	return true;
}

void
Shader::printInfoLog( GLuint obj )
{
	char* infoLog = 0;
	int infologLength = 0;
	int charsWritten  = 0;

	glGetProgramiv( obj , GL_INFO_LOG_LENGTH, (GLint*) &infologLength );
	if (infologLength > 0)
	{
		glGetShaderInfoLog( obj , infologLength, (GLint*) &charsWritten, infoLog );

		//glGetProgramInfoLog( obj, infologLength, (GLint*) &charsWritten, infoLog );

	    if ( charsWritten )
			printf("%s\n",infoLog );

	    free( infoLog );
	}
}
