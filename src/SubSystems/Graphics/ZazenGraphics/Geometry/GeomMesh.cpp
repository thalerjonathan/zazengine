#include "GeomMesh.h"

#define BUFFER_OFFSET( i ) ( ( char* ) NULL + ( i ) )

GeomMesh::GeomMesh( int faceCount, int vertexCount, VertexData* data, int* indexBufer )
	: faceCount(faceCount),
	  vertexCount( vertexCount )
{
	this->dataVBO = 0;
	this->indexVBO = 0;

	this->data = data;
	this->indexBuffer = indexBuffer;
}

GeomMesh::~GeomMesh()
{
	if (this->dataVBO )
	{
		glDeleteBuffers( 1, &this->dataVBO );
		glDeleteBuffers( 1, &this->indexVBO );

		if ( this->data )
			delete this->data;

		if ( this->indexBuffer )
			delete this->indexBuffer;
	}
}

void GeomMesh::render()
{
	GeomType::render();
	
	if ( 0 != this->dataVBO )
	{
		glGenBuffers( 1, &this->dataVBO );
		glBindBuffer( GL_ARRAY_BUFFER, this->dataVBO );
		glBufferData( GL_ARRAY_BUFFER, sizeof( VertexData ) * this->vertexCount, this->data, GL_STATIC_DRAW );

		// index-buffers are always 3*facecount => each face has 3 vertices
		glGenBuffers( 1, &this->indexVBO );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, this->indexVBO );
		glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( int ) * this->faceCount * 3, this->indexBuffer, GL_STATIC_DRAW );
	}

	glBindBuffer(GL_ARRAY_BUFFER, this->dataVBO );
	glEnableVertexAttribArray(0);
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof( VertexData ), BUFFER_OFFSET( 0 ) );
	glEnableVertexAttribArray(1);
	glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof( VertexData ), BUFFER_OFFSET( 12 ) );

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, this->indexVBO );
	glDrawElements( GL_TRIANGLES, this->faceCount * 3, GL_INT, BUFFER_OFFSET( 0 ) );
}
