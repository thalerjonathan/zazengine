#ifndef GEOMSPHERE_H_
#define GEOMSPHERE_H_

#include <GL/glew.h>
#include <GL/glut.h>

#include "GeomType.h"

class GeomSphere : public GeomType
{
 public:
	 GeomSphere(float radius) { this->radius = radius; };
	virtual ~GeomSphere() {};
 
	bool render() { glutSolidSphere(this->radius, 50, 50); return true; };

 private:
	float radius;
	
};

#endif /*SPHERENODE_H_*/
