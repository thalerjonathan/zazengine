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

	const std::string& getName() { return this->m_name; };
	GLuint getID() { return this->m_id; };

	GLuint getBinding() { return this->m_binding; };

	bool bind();

	bool bindBuffer();
	bool unbindBuffer();

	bool updateData( const void* data, int size );
	bool updateData( const void* data, int offset, int size );

 private:
	UniformBlock( const std::string& name );

	static GLuint nextBinding;

	GLuint m_id;
	const std::string m_name;

	GLuint m_binding;

};

#endif /* UNIFORMBLOCK_H_ */
