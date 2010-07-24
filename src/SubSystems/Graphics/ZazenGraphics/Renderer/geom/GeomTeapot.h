#ifndef GEOMTEAPOT_H_
#define GEOMTEAPOT_H_

#include <GL/glew.h>
#include <GL/glut.h>

#include "GeomType.h"

class GeomTeapot : public GeomType
{
 public:
	GeomTeapot(float size) { this->size = size; };
	virtual ~GeomTeapot() {};
 
	void render() { glutSolidTeapot(this->size); };

 private:
	float size;
	
};

#endif /*TEAPOTNODE_H_*/
