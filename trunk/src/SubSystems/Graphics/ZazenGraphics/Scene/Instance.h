#ifndef INSTANCE_H_
#define INSTANCE_H_

#include "../Geometry/GeomType.h"
#include "../../../../Core/Utils/Math/Orientation.h"

class Instance : public Orientation
{
 public:
	Instance();
	~Instance();

	bool visible;
	float distance;
	long lastFrame;

	GeomType* geom;

	glm::mat4 m_modelMatrix;

	float recalculateDistance();

};

#endif /*GEOMINSTANCENODE_H_*/
