#ifndef GEOMBOX_H_
#define GEOMBOX_H_

#include <GL/glut.h>

#include "GeomType.h"

class GeomBox : public GeomType
{
 public:
	GeomBox(float size) { this->size = size; };
	virtual ~GeomBox() {};
 
	bool render() { glutSolidCube(this->size); return true; };

 private:
	float size;
	
};

#endif /*BOXNODE_H_*/
