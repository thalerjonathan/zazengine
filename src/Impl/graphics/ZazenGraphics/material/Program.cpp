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
Program::createProgram( const std::string& programName )
{
	GLint status;
	GLuint programObject = 0;

	programObject = glCreateProgram();
	if ( 0 == programObject )
	{
		if ( GL_NO_ERROR != ( status = glGetError() ) )
			cout << "ERROR ... in Program::createProgram for programm " << programName << ": glCreateProgram failed with " << gluErrorString( status ) << endl;

		return 0;
	}

	return new Program( programObject, programName );
}

Program::Program( GLuint programObject, const std::string& programName )
{
	this->m_programObject = programObject;
	this->m_programName = programName;
}

Program::~Program()
{
	glDeleteProgram( this->m_programObject );
}

void
Program::printInfoLog()
{
	GLchar* infoLog = 0;
	GLint infoLogLen = 0;
	GLint charsWritten  = 0;

	glGetProgramiv( this->m_programObject , GL_INFO_LOG_LENGTH, &infoLogLen );
	if (infoLogLen > 0)
	{
		infoLog = (GLchar*) malloc( ( infoLogLen + 1 ) * sizeof( GLchar ) );
		memset( infoLog, 0, infoLogLen + 1 );

		glGetProgramInfoLog( this->m_programObject, infoLogLen, &charsWritten, infoLog );

	    if ( charsWritten )
			cout << infoLog << endl;

	    free( infoLog );
	}
}

bool
Program::attachShader( Shader* shader )
{
	GLint status;

	glAttachShader( this->m_programObject, shader->getObject() );
	if ( GL_NO_ERROR != ( status = glGetError() ) )
	{
		cout << "ERROR ... in Program::attachShader for programm " << this->m_programName << ": glAttachShader failed with " << gluErrorString( status ) << endl;
		return false;
	}

	return true;
}


bool
Program::detachShader( Shader* shader )
{
	GLint status;

	glDetachShader( this->m_programObject, shader->getObject() );
	if ( GL_NO_ERROR != ( status = glGetError() ) )
	{
		cout << "ERROR ... in Program::detachShader for programm " << this->m_programName << ": glDetachShader failed with " << gluErrorString( status ) << endl;
		return false;
	}

	return true;
}

bool
Program::link()
{
	GLint status;

	glLinkProgram( this->m_programObject );
	glGetProgramiv( this->m_programObject, GL_LINK_STATUS, &status);
	if ( GL_TRUE != status )
	{
		cout << "ERROR ... in Program::link for programm " << this->m_programName << ": linking of program failed. Error-Log:" << endl;
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

	glGetActiveUniformBlockiv( this->m_programObject, index, GL_UNIFORM_BLOCK_DATA_SIZE, &uniformBlockSize );

	// create the UBO
	if ( false == block->updateData( 0, uniformBlockSize ) )
		return false;

	//if ( false == block->bind( index ) )
	//	return false;

	glUniformBlockBinding( this->m_programObject, index,  block->getBindIndex() );
	if ( GL_NO_ERROR != ( status = glGetError() ) )
	{
		cout << "ERROR ... in Program::bindUniformBlock for programm " << this->m_programName << ": glUniformBlockBinding failed for name \"" << block->getName() << "\": " << gluErrorString( status )  << endl;
		return false;
	}

	return true;
}

bool
Program::bindAttribLocation( GLuint index, const std::string& name )
{
	GLint status;

	glBindAttribLocation( this->m_programObject, index, name.c_str() );
	if ( GL_NO_ERROR != ( status = glGetError() ) )
	{
		cout << "ERROR ... in Program::bindAttribLocation for programm " << this->m_programName << ": glBindAttribLocation failed for name \"" << name << "\": " << gluErrorString( status )  << endl;
		return false;
	}

	return true;
}

GLint
Program::getAttribLocation( const std::string& name )
{
	GLint location = 0;

	location = glGetAttribLocation( this->m_programObject, name.c_str() );
	if ( -1 == location )
		cout << "ERROR ... in Program::getAttribLocation for programm " << this->m_programName << ": coulnd't glGetAttribLocation location for name \"" << name << "\": " << gluErrorString( glGetError() )  << endl;

	return location;
}

bool
Program::bindFragDataLocation( GLuint index, const std::string& name )
{
	GLint status;

	glBindFragDataLocation( this->m_programObject, index, name.c_str() );
	if ( GL_NO_ERROR != ( status = glGetError() ) )
	{
		cout << "ERROR ... in Program::bindFragDataLocation for programm " << this->m_programName << ": glBindFragDataLocation failed for name \"" << name << "\": " << gluErrorString( status )  << endl;
		return false;
	}

	return true;
}

GLint
Program::getFragDataLocation( const std::string& name )
{
	GLint location = 0;

	location = glGetFragDataLocation( this->m_programObject, name.c_str() );
	if ( -1 == location )
		cout << "ERROR ... in Program::getFragDataLocation for programm " << this->m_programName << ": coulnd't glGetFragDataLocation location for name \"" << name << "\": " << gluErrorString(glGetError())  << endl;

	return location;
}

bool
Program::use()
{
	GLint status;

	glUseProgram( this->m_programObject );
	if ( GL_NO_ERROR != ( status = glGetError() ) )
	{
		cout << "ERROR ... in Program::use for programm " << this->m_programName << ": glUseProgram failed: " << gluErrorString( status )  << endl;
		return false;
	}

	return true;
}

bool
Program::unuse()
{
	GLint status;

	glUseProgram( 0 );
	if ( GL_NO_ERROR != ( status = glGetError() ) )
	{
		cout << "ERROR ... in Program::unuse: glUseProgram( 0 ) failed: " << gluErrorString( status )  << endl;
		return false;
	}

	return true;
}

bool
Program::setUniform4( const std::string& name, const float* value )
{
	GLint status;
	GLint location = this->getUniformLocation( name );
	if ( -1 == location )
		return false;

	glUniform4fv( location, 4, value );
	if ( GL_NO_ERROR != ( status = glGetError() ) )
	{
		cout << "ERROR ... in Program::setUniform4 for programm " << this->m_programName << ": glUniform4fv failed for " << name << ": " << gluErrorString( status )  << endl;
		return false;
	}

	return true;
}

bool
Program::setUniformInt( const std::string& name, int value )
{
	GLint status;
	GLint location = this->getUniformLocation( name );
	if ( -1 == location )
		return false;

	glUniform1i( location, value );
	if ( GL_NO_ERROR != ( status = glGetError() ) )
	{
		cout << "ERROR ... in Program::setUniformInt for programm " << this->m_programName << ": glUniform1i failed for " << name << ": " << gluErrorString( status )  << endl;
		return false;
	}

	return true;
}

bool
Program::setUniformMatrix4( const std::string& name, const float* value )
{
	GLint status;

	GLint location = this->getUniformLocation( name );
	if ( -1 == location )
		return false;

	glUniformMatrix4fv( location, 1, GL_FALSE, value );
	if ( GL_NO_ERROR != ( status = glGetError() ) )
	{
		cout << "ERROR ... in Program::setUniformMatrix4 for programm " << this->m_programName << ": glUniformMatrix4fv failed for " << name << ": " << gluErrorString( status )  << endl;
		return false;
	}

	return true;
}

GLuint
Program::getUniformBlockIndex( const std::string& name )
{
	GLuint index = 0;

	index = glGetUniformBlockIndex( this->m_programObject, name.c_str() );
	if ( GL_INVALID_INDEX == index )
		cout << "ERROR ... in Program::getUniformBlockIndex for programm " << this->m_programName << ": glGetUniformBlockIndex failed for name \"" << name << "\". OpenGL-Error: GL_INVALID_INDEX" << endl;

	return index;
}

GLint
Program::getUniformLocation( const std::string& name )
{
	GLint location = 0;

	location = glGetUniformLocation( this->m_programObject, name.c_str() );
	if ( -1 == location )
		cout << "ERROR ... in Program::getUniformLocation for programm " << this->m_programName << ": coulnd't get Uniform Location for name \"" << name << "\": " << gluErrorString( glGetError() )  << endl;

	return location;
}
