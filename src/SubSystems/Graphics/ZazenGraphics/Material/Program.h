/*
 * Program.h
 *
 *  Created on: Jan 16, 2011
 *      Author: jonathan
 */

#ifndef PROGRAM_H_
#define PROGRAM_H_

#include "Shader.h"

class Program
{
 public:
	~Program();

	static Program* createProgram();

	bool attachShader( Shader* );
	bool detachShader( Shader* );

	bool bindFragDataLocation( GLuint colorNumber, const std::string& name );

	bool link();

	bool activate();
	bool deactivate();

 private:
	Program( GLuint programObject );

	GLuint programObject;

	static void printInfoLog( GLuint obj );
};

#endif /* PROGRAM_H_ */
