/*
 * Program.cpp
 *
 *  Created on: Jan 16, 2011
 *      Author: jonathan
 */

#include "Program.h"

#include <stdlib.h>
#include <stdio.h>

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
		printInfoLog( this->programObject );
		return false;
	}

	return true;
}

bool
Program::bindFragDataLocation(GLuint colorNumber, const std::string& name )
{
	glBindFragDataLocation( this->programObject, colorNumber, name.c_str() );

	GLenum status = glGetError();
	if ( GL_NO_ERROR != status )
	{
		cout << "glBindFragDataLocation failed for name \"" << name << "\": " << gluErrorString( status )  << endl;
		return false;
	}

	return true;
}

bool
Program::activate()
{
	glUseProgram( this->programObject );

	/*glUniform1i(loc, value);

		status = glGetError();
		if ( GL_NO_ERROR != status )
		{
			cout << "glUniform1i failed: " << gluErrorString( status )  << endl;
			return false;
		}

		uniIter++;
	}
	*/

	return true;
}

bool
Program::deactivate()
{
	glUseProgram(0);

	return true;
}

/*
GLint
Program::queryUniformLoc(GLint prog, const GLchar* name)
{
	GLint location = 0;

	location = glGetUniformLocation(prog, name);
	if (location == -1)
	{
		cout << "Coulnd't get Uniform Location for name \"" << name << "\". OpenGL-Error: " << gluErrorString(glGetError())  << endl;
		return -1;
	}

	return location;
}
*/

void
Program::printInfoLog( GLuint obj )
{
	char* infoLog = 0;
	int infologLength = 0;
	int charsWritten  = 0;

	glGetProgramiv( obj , GL_INFO_LOG_LENGTH, (GLint*) &infologLength );
	if (infologLength > 0)
	{
		glGetProgramInfoLog( obj, infologLength, (GLint*) &charsWritten, infoLog );

	    if ( charsWritten )
			printf("%s\n",infoLog);

	    free( infoLog );
	}
}
