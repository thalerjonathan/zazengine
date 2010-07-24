#ifndef PHYSICTYPE_H_
#define PHYSICTYPE_H_

#include <ode/ode.h>

#include "../../../../Core/Utils/Math/Vector.h"

class PhysicType
{
 public:
	 PhysicType(bool staticFlag, float mass) { this->staticFlag = staticFlag; this->mass = mass; };
	 virtual ~PhysicType()
	 { 
		 if (this->bodyID)
			dBodyDestroy(this->bodyID);
		 
		 dGeomDestroy(this->geomID);
	 };
	 
	 bool isStatic() { return this->staticFlag; };
	 float getMass() { return this->mass; };

	 void setPosition(const Vector& pos)
	 {
		 if (this->staticFlag)
			 dGeomSetPosition(this->geomID, pos[0], pos[1], pos[2]);
		 else
			 dBodySetPosition(this->bodyID, pos[0], pos[1], pos[2]);
	 };

	 dBodyID getBodyID() { return this->bodyID; };

	 virtual bool create(dWorldID, dSpaceID) = 0;
	 
 protected:
	 bool staticFlag;
	 float mass;
	 
	dBodyID bodyID;
	dGeomID geomID;

};

#endif /*PHYSICTYPE_H_*/
