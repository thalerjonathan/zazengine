/*
 * UniformBlock.h
 *
 *  Created on: Jan 31, 2011
 *      Author: jonathan
 */

#ifndef UNIFORMBLOCK_H_
#define UNIFORMBLOCK_H_

#include <GL/glew.h>

#include <string>

class UniformBlock
{
 public:
	static UniformBlock* createBlock( const std::string& name );

	virtual ~UniformBlock();

	const std::string& getName() { return this->name; };
	GLuint getID() { return this->id; };

	bool bind( int index );

	bool bindBuffer();
	bool unbindBuffer();

	bool updateData( void* data, int size );
	bool updateData( void* data, int offset, int size );

 private:
	UniformBlock( const std::string& name );

	GLuint id;
	const std::string name;

};

#endif /* UNIFORMBLOCK_H_ */
