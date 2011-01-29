#ifndef MESHNODE_H_
#define MESHNODE_H_

#include <GL/glew.h>

#include "GeomType.h"

class GeomMesh : public GeomType
{
 public:
	struct VertexData
	{
		float position[3];
		float normal[3];
	};

	GeomMesh( int faceCount, int vertexCount, VertexData* data, int* indexBufer );
	virtual ~GeomMesh();

	virtual int getFaceCount() { return this->faceCount; };

	void render();
	
 private:
	int faceCount;
	int vertexCount;

	int* indexBuffer;
	VertexData* data;

	GLuint dataVBO;
	GLuint indexVBO;

};

#endif /*MESHNODE_H_*/
