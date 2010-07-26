#ifndef GEOMINSTANCE_H_
#define GEOMINSTANCE_H_

#include "GeomType.h"
#include "../Material/Material.h"
#include "../../../../Core/Utils/Math/Transform.h"

class GeomInstance
{
	public:
		GeomInstance( GeomType* );
		~GeomInstance();
	 
		 bool visible;
		 float distance;
		 long lastFrame;

		 GeomType* geom;

		 GeomInstance* parent;
		 std::vector<GeomInstance*> children;

		 Matrix transform;
		 std::vector<Transform*> transforms;

		 float recalculateDistance();
};

#endif /*GEOMINSTANCENODE_H_*/
