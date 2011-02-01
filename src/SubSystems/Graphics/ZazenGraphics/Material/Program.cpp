/*
 * Program.cpp
 *
 *  Created on: Jan 16, 2011
 *      Author: jonathan
 */

#include "Program.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <iostream>

using namespace std;

Program*
Program::createProgram()
{
	GLint status;
	GLuint programObject = 0;

	programObject = glCreateProgram();
	if ( 0 == programObject )
	{
		status = glGetError();
		if ( GL_NO_ERROR != status )
			cout << "glCreateProgram failed with " << gluErrorString( status ) << endl;

		return 0;
	}

	return new Program( programObject );
}

Program::Program( GLuint programObject )
{
	this->programObject = programObject;
}

Program::~Program()
{
	glDeleteProgram( this->programObject );
}

void
Program::printInfoLog()
{
	GLchar* infoLog = 0;
	GLint infoLogLen = 0;
	GLint charsWritten  = 0;

	glGetProgramiv( this->programObject , GL_INFO_LOG_LENGTH, &infoLogLen );
	if (infoLogLen > 0)
	{
		infoLog = (GLchar*) malloc( ( infoLogLen + 1 ) * sizeof( GLchar ) );
		memset( infoLog, 0, infoLogLen + 1 );

		glGetProgramInfoLog( this->programObject, infoLogLen, &charsWritten, infoLog );

	    if ( charsWritten )
			cout << infoLog << endl;

	    free( infoLog );
	}
}

bool
Program::attachShader( Shader* shader )
{
	GLint status;

	glAttachShader( this->programObject, shader->getObject() );
	status = glGetError();
	if ( GL_NO_ERROR != status )
	{
		cout << "glAttachShader failed with " << gluErrorString( status ) << endl;
		return false;
	}

	return true;
}


bool
Program::detachShader( Shader* shader )
{
	GLint status;

	glDetachShader( this->programObject, shader->getObject() );
	status = glGetError();
	if ( GL_NO_ERROR != status )
	{
		cout << "glDetachShader failed with " << gluErrorString( status ) << endl;
		return false;
	}

	return true;
}

bool
Program::link()
{
	GLint status;

	glLinkProgram( this->programObject );
	glGetProgramiv( this->programObject, GL_LINK_STATUS, &status);
	if ( GL_TRUE != status )
	{
		cout << "ERROR linking of program failed" << endl;
		this->printInfoLog();
		return false;
	}

	return true;
}

bool
Program::bindUniformBlock( UniformBlock* block )
{
	GLint status;
	GLint uniformBlockSize;

	GLuint index = this->getUniformBlockIndex( block->getName() );
	if ( GL_INVALID_INDEX == index )
		return false;

	glGetActiveUniformBlockiv( this->programObject, index, GL_UNIFORM_BLOCK_DATA_SIZE, &uniformBlockSize);

	// create the UBO
	if ( false == block->updateData( 0, uniformBlockSize ) )
		return false;

	if ( false == block->bind( 0 ) )
		return false;

	glUniformBlockBinding( this->programObject, index, 0 );
	status = glGetError();
	if ( GL_NO_ERROR != status )
	{
		cout << "glUniformBlockBinding failed for name \"" << block->getName() << "\": " << gluErrorString( status )  << endl;
		return false;
	}

	return true;
}

bool
Program::bindAttribLocation( GLuint index, const std::string& name )
{
	GLint status;

	glBindAttribLocation( this->programObject, index, name.c_str() );
	status = glGetError();
	if ( GL_NO_ERROR != status )
	{
		cout << "glBindAttribLocation failed for name \"" << name << "\": " << gluErrorString( status )  << endl;
		return false;
	}

	return true;
}

bool
Program::bindFragDataLocation( GLuint index, const std::string& name )
{
	glBindFragDataLocation( this->programObject, index, name.c_str() );

	GLenum status = glGetError();
	if ( GL_NO_ERROR != status )
	{
		cout << "glBindFragDataLocation failed for name \"" << name << "\": " << gluErrorString( status )  << endl;
		return false;
	}

	return true;
}

bool
Program::use()
{
	glUseProgram( this->programObject );

	GLint status = glGetError();
	if ( GL_NO_ERROR != status )
	{
		cout << "glUseProgram failed: " << gluErrorString( status )  << endl;
		return false;
	}

	return true;
}

bool
Program::setUniform4( const std::string& name, const float* value )
{
	GLint location = this->getUniformLocation( name );
	if ( -1 == location )
		return false;

	glUniform4fv( location, 4, value );

	GLint status = glGetError();
	if ( GL_NO_ERROR != status )
	{
		cout << "glUniform4fv failed for " << name << ": " << gluErrorString( status )  << endl;
		return false;
	}

	return true;
}

bool
Program::setUniformMatrix4( const std::string& name, const float* value )
{
	GLint location = this->getUniformLocation( name );
	if ( -1 == location )
		return false;

	glUniformMatrix4fv( location, 1, GL_FALSE, value );

	GLint status = glGetError();
	if ( GL_NO_ERROR != status )
	{
		cout << "glUniformMatrix4fv failed for " << name << ": " << gluErrorString( status )  << endl;
		return false;
	}

	return true;
}

GLuint
Program::getUniformBlockIndex( const std::string& name )
{
	GLuint index = 0;

	index = glGetUniformBlockIndex( this->programObject, name.c_str() );
	if ( GL_INVALID_INDEX == index )
		cout << "glGetUniformBlockIndex failed for name \"" << name << "\". OpenGL-Error: GL_INVALID_INDEX" << endl;

	return index;
}

GLint
Program::getUniformLocation( const std::string& name )
{
	GLint location = 0;

	location = glGetUniformLocation( this->programObject, name.c_str() );
	if ( -1 == location )
		cout << "Coulnd't get Uniform Location for name \"" << name << "\". OpenGL-Error: " << gluErrorString(glGetError())  << endl;

	return location;
}
