#ifndef GEOMINSTANCE_H_
#define GEOMINSTANCE_H_

#include "geom/GeomType.h"
#include "Material.h"
#include "../../../../Utils/Math/Transform.h"

class GeomInstance
{
 public:
	 GeomInstance(GeomType*);
	 ~GeomInstance();
	 
	 bool visible;
	 float distance;
	 long lastFrame;

	 GeomType* geom;
	 
	 float recalculateDistance();
  
	 GeomInstance* parent;
	 std::vector<GeomInstance*> children;
	 
	 Matrix transform;
	 std::vector<Transform*> transforms;
};

#endif /*GEOMINSTANCENODE_H_*/
