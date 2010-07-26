#ifndef MESHNODE_H_
#define MESHNODE_H_

#include <GL/glew.h>

#include "GeomType.h"

class GeomMesh : public GeomType
{
 public:
	typedef float Vertex[3];

	GeomMesh(int faces, Vertex*, Vertex*);
	virtual ~GeomMesh();

	virtual int getFaceCount() { return this->faces; };

	void render();
	
 private:
	int faces;

	Vertex* vertices;
	Vertex* normals;
 
	GLuint vertexVBO;
	GLuint normalVBO;

};

#endif /*MESHNODE_H_*/
