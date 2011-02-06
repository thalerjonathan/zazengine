#ifndef INSTANCE_H_
#define INSTANCE_H_

#include "../Geometry/GeomType.h"

class Instance
{
 public:
	Instance( GeomType* );
	~Instance();
	 
	bool visible;
	float distance;
	long lastFrame;

	GeomType* geom;

	glm::mat4* m_modelMatrix;

	float recalculateDistance();

};

#endif /*GEOMINSTANCENODE_H_*/
