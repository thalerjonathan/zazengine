/*
 * Program.h
 *
 *  Created on: Jan 16, 2011
 *      Author: jonathan
 */

#ifndef PROGRAM_H_
#define PROGRAM_H_

#include <GL/glew.h>

#include <map>
#include <string>

class Program
{
 public:
	~Program();

	static Program* createProgram(const std::string&, const std::string&);

	bool putUniform( const std::string&, int value );

	bool activate();
	bool deactivate();

 private:
	Program();

	GLuint materialProgram;
	GLuint vertexShader;
	GLuint fragmentShader;

	std::map<GLint, int> uniforms;

	static bool readShaderSource(const std::string&, std::string&);
	static GLint queryUniformLoc(GLint prog, const GLchar* name);
};

#endif /* PROGRAM_H_ */
