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

void
printInfoLog( GLuint obj, bool shader )
{
	char* infoLog = 0;
	int infologLength = 0;
	int charsWritten  = 0;

	glGetProgramiv( obj, GL_INFO_LOG_LENGTH, (GLint*) &infologLength );
	if (infologLength > 0)
	{
		infoLog = (char*) malloc( infologLength );
		if ( shader )
			glGetShaderInfoLog( obj, infologLength, (GLint*) &charsWritten, infoLog );
		else
			glGetProgramInfoLog( obj, infologLength, (GLint*) &charsWritten, infoLog );

	    if (charsWritten)
			printf("%s\n",infoLog);

	    free(infoLog);
	}
}

Program*
Program::createProgram(const string& vertexSourceFile, const string& fragmentSourceFile)
{
	GLint status;
	GLuint program = 0;
	GLuint vertexShader = 0;
	GLuint fragmentShader = 0;

	string vertexSourceStr;
	string fragmentSourceStr;

	if (Program::readShaderSource(vertexSourceFile, vertexSourceStr) == false)
		return 0;

	if (Program::readShaderSource(fragmentSourceFile, fragmentSourceStr) == false)
		return 0;

	const char* vertexSource = vertexSourceStr.c_str();
	const char* fragmentSource = fragmentSourceStr.c_str();

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	if (0 == vertexShader )
	{
		cout << "glCreateShader for GL_VERTEX_SHADER \"" << vertexSourceFile << "\" failed with " << gluErrorString(glGetError()) << endl;
		return 0;
	}

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	if ( 0 == fragmentShader )
	{
		cout << "glCreateShader for GL_FRAGMENT_SHADER \"" << fragmentSourceFile << "\" failed with " << gluErrorString(glGetError()) << endl;
		return 0;
	}

	glShaderSource(vertexShader, 1, (const GLchar**) &vertexSource, NULL);
	status = glGetError();
	if ( GL_NO_ERROR != status )
	{
		cout << "glShaderSource for GL_VERTEX_SHADER \"" << vertexSourceFile << "\" failed with " << gluErrorString( status ) << endl;
		return 0;
	}

	glShaderSource(fragmentShader, 1, (const GLchar**) &fragmentSource, NULL);
	status = glGetError();
	if ( GL_NO_ERROR != status )
	{
		cout << "glShaderSource for GL_FRAGMENT_SHADER \"" << fragmentSourceFile << "\" failed with " << gluErrorString( status ) << endl;
		return 0;
	}

	glCompileShader( vertexShader );
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
	if ( GL_TRUE != status )
	{
		cout << "Failed compiling GL_VERTEX_SHADER \"" << vertexSourceFile << "\"" << endl;
		printInfoLog( vertexShader, true );
		return 0;
	}

	glCompileShader( fragmentShader );
	glGetShaderiv( fragmentShader, GL_COMPILE_STATUS, &status );
	if ( GL_TRUE != status )
	{
		cout << "Failed compiling GL_FRAGMENT_SHADER \"" << fragmentSourceFile << "\"" << endl;
		printInfoLog( fragmentShader, true );
		return 0;
	}

	program = glCreateProgram();
	if ( 0 == program )
	{
		status = glGetError();
		if ( GL_NO_ERROR != status )
			cout << "glCreateProgram failed with " << gluErrorString( status ) << endl;

		return 0;
	}

	glAttachShader( program, vertexShader );
	status = glGetError();
	if ( GL_NO_ERROR != status )
	{
		cout << "glCompileShader for GL_VERTEX_SHADER \"" << vertexSourceFile << "\" failed with " << gluErrorString( status ) << endl;
		return 0;
	}

	glAttachShader( program, fragmentShader );
	status = glGetError();
	if ( GL_NO_ERROR != status )
	{
		cout << "glCompileShader for GL_FRAGMENT_SHADER \"" << fragmentSourceFile << "\" failed with " << gluErrorString( status ) << endl;
		return 0;
	}

	Program* prog = new Program();
	prog->program = program;
	prog->vertexShader = vertexShader;
	prog->fragmentShader = fragmentShader;
	prog->fragmentSourceFile = fragmentSourceFile;
	prog->vertexSourceFile = vertexSourceFile;

	return prog;
}

Program::Program()
{
	this->program = 0;
	this->vertexShader = 0;
	this->fragmentShader = 0;
}

Program::~Program()
{

}


bool
Program::link()
{
	GLint status;

	glLinkProgram( this->program );
	glGetProgramiv( this->program, GL_LINK_STATUS, &status);
	if ( GL_TRUE != status )
	{
		cout << "ERROR linking of program \"" << this->vertexSourceFile << "\" and \"" << this->fragmentSourceFile << "\" failed" << endl;
		printInfoLog( this->program, false );
		return false;
	}

	return true;
}

bool
Program::putUniform( const std::string& id, int value )
{
	GLint loc = Program::queryUniformLoc( this->program, id.c_str());
	if (loc != -1)
		this->uniforms[loc] = value;

	return true;
}

bool
Program::bindFragDataLocation(GLuint colorNumber, const std::string& name )
{
	glBindFragDataLocation( this->program, colorNumber, name.c_str() );

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
	GLenum status;

	glUseProgram(this->program);

	map<GLint, int>::iterator uniIter = this->uniforms.begin();
	while (uniIter != this->uniforms.end()) {
		GLint loc = uniIter->first;
		int value = uniIter->second;

		glUniform1i(loc, value);

		status = glGetError();
		if ( GL_NO_ERROR != status )
		{
			cout << "glUniform1i failed: " << gluErrorString( status )  << endl;
			return false;
		}

		uniIter++;
	}

	return true;
}

bool
Program::deactivate()
{
	glUseProgram(0);

	return true;
}

bool
Program::readShaderSource(const string& file, string& shaderSource)
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
