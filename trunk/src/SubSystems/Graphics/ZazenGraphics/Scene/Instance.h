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

	Transform transform;

	float recalculateDistance();

};

#endif /*GEOMINSTANCENODE_H_*/
