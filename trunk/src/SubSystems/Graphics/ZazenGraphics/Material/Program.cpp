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
printShaderInfoLog(GLuint obj)
{
	int infologLength = 0;
	int charsWritten = 0;
	char* infoLog;

	glGetShaderiv(obj, GL_INFO_LOG_LENGTH, (GLint*) &infologLength);

	if (infologLength > 0) {
		infoLog = (char*) malloc(infologLength);
		glGetShaderInfoLog(obj, infologLength, (GLint*) &charsWritten, infoLog);
		if (charsWritten)
			printf("%s\n",infoLog);
		free(infoLog);
	}
}

void
printProgramInfoLog(GLuint obj)
{
	int infologLength = 0;
	int charsWritten  = 0;
	char *infoLog;

	glGetProgramiv(obj, GL_INFO_LOG_LENGTH, (GLint*) &infologLength);
	if (infologLength > 0) {
	        infoLog = (char*) malloc(infologLength);
	        glGetProgramInfoLog(obj, infologLength, (GLint*) &charsWritten, infoLog);
		if (charsWritten)
			printf("%s\n",infoLog);
	        free(infoLog);
	}
}

Program::Program()
{
	this->materialProgram = 0;
	this->vertexShader = 0;
	this->fragmentShader = 0;
}

Program::~Program()
{

}

bool
Program::activate()
{
	glUseProgram(this->materialProgram);

	map<GLint, int>::iterator uniIter = this->uniforms.begin();
	while (uniIter != this->uniforms.end()) {
		GLint loc = uniIter->first;
		int value = uniIter->second;

		glUniform1i(loc, value);

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
Program::putUniform( const std::string& id, int value )
{
	GLint loc = Program::queryUniformLoc(this->materialProgram, id.c_str());
	if (loc != -1)
		this->uniforms[loc] = value;

	return true;
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
	if (vertexShader == 0) {
		cout << "glCreateShader for GL_VERTEX_SHADER \"" << vertexSourceFile << "\" failed with " << gluErrorString(glGetError()) << endl;
		return 0;
	}

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	if (fragmentShader == 0)
	{
		cout << "glCreateShader for GL_FRAGMENT_SHADER \"" << fragmentSourceFile << "\" failed with " << gluErrorString(glGetError()) << endl;
		return 0;
	}

	glShaderSource(vertexShader, 1, (const GLchar**) &vertexSource, NULL);
	if (glGetError() != GL_NO_ERROR)
	{
		cout << "glShaderSource for GL_VERTEX_SHADER \"" << vertexSourceFile << "\" failed with " << gluErrorString(glGetError()) << endl;
		return 0;
	}

	glShaderSource(fragmentShader, 1, (const GLchar**) &fragmentSource, NULL);
	if (glGetError() != GL_NO_ERROR)
	{
		cout << "glShaderSource for GL_FRAGMENT_SHADER \"" << fragmentSourceFile << "\" failed with " << gluErrorString(glGetError()) << endl;
		return 0;
	}

	glCompileShader(vertexShader);
	if (glGetError() != GL_NO_ERROR)
		cout << "glCompileShader for GL_VERTEX_SHADER \"" << vertexSourceFile << "\" failed with " << gluErrorString(glGetError()) << endl;

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
	printShaderInfoLog(vertexShader);
	if (!status)
	{
		cout << "Failed compiling GL_VERTEX_SHADER \"" << vertexSourceFile << "\"" << endl;
		return 0;
	}

	glCompileShader(fragmentShader);
	if (glGetError() != GL_NO_ERROR)
		cout << "glCompileShader for GL_FRAGMENT_SHADER \"" << fragmentSourceFile << "\" failed with " << gluErrorString(glGetError()) << endl;
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
	printShaderInfoLog(fragmentShader);
	if (!status) {
		cout << "Failed compiling GL_FRAGMENT_SHADER \"" << fragmentSourceFile << "\"" << endl;
		return 0;
	}

	program = glCreateProgram();
	if (program == 0) {
		if (glGetError() != GL_NO_ERROR)
			cout << "glCreateProgram failed with " << gluErrorString(glGetError()) << endl;

		return 0;
	}

	glAttachShader(program, vertexShader);
	if (glGetError() != GL_NO_ERROR)
		cout << "glCompileShader for GL_VERTEX_SHADER \"" << vertexSourceFile << "\" failed with " << gluErrorString(glGetError()) << endl;
	glAttachShader(program, fragmentShader);
	if (glGetError() != GL_NO_ERROR)
		cout << "glCompileShader for GL_FRAGMENT_SHADER \"" << fragmentSourceFile << "\" failed with " << gluErrorString(glGetError()) << endl;

	glLinkProgram(program);
	if (glGetError() != GL_NO_ERROR)
		cout << "glLinkProgramARB failed with " << gluErrorString(glGetError()) << endl;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	printProgramInfoLog(program);
	if (!status) {
		cout << "ERROR linking of program \"" << vertexSourceFile << "\" and \"" << fragmentSourceFile << "\" failed" << endl;
		return 0;
	}

	Program* prog = new Program();
	prog->materialProgram = program;
	prog->vertexShader = vertexShader;
	prog->fragmentShader = fragmentShader;

	return prog;
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

	glGetError();

	location = glGetUniformLocation(prog, name);
	if (location == -1) {
		cout << "Coulnd't get Uniform Location for name \"" << name << "\". OpenGL-Error: " << gluErrorString(glGetError())  << endl;
		return -1;
	}

	return location;
}
