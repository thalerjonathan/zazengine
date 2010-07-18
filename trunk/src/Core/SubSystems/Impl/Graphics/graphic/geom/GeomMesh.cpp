#include "GeomMesh.h"

GeomMesh::GeomMesh(int faces, Vertex* vertices, Vertex* normals)
	: faces(faces)
{
	this->vertexVBO = 0;
	this->normalVBO = 0;
	
	this->vertices = vertices;
	this->normals = normals;
}

GeomMesh::~GeomMesh()
{
	if (this->vertexVBO != 0 && this->normalVBO != 0) {
		// TODO: release data from GPU
	}
}

void GeomMesh::render()
{
	GeomType::render();
	
	if (this->vertexVBO == 0 && this->normalVBO == 0) {
		glGenBuffers(1, &this->vertexVBO);
		glBindBuffer(GL_ARRAY_BUFFER, this->vertexVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 3 * this->faces, this->vertices, GL_STATIC_DRAW);
	
		glGenBuffers(1, &this->normalVBO);
		glBindBuffer(GL_ARRAY_BUFFER, this->normalVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 3 * this->faces, this->normals, GL_STATIC_DRAW);
	}
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, this->normalVBO);
	glNormalPointer(GL_FLOAT, 0, NULL);
 
	glBindBuffer(GL_ARRAY_BUFFER, this->vertexVBO);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glDrawArrays(GL_TRIANGLES, 0, this->faces * 3);
	
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
}
