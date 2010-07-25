#include "PhysicSphere.h"

PhysicSphere::PhysicSphere(bool staticFlag, float mass, float radius)
	: PhysicType(staticFlag, mass),
	radius(radius)
{
}

PhysicSphere::~PhysicSphere()
{
}

bool PhysicSphere::create(dWorldID worldID, dSpaceID spaceID)
{
	this->bodyID = 0;
	this->geomID = dCreateSphere(spaceID, radius);
	
	if (this->staticFlag == false) {
		this->bodyID = dBodyCreate(worldID);
		
		dMass massStruct;
		dMassSetZero(&massStruct);
		dMassSetSphereTotal(&massStruct, this->mass, radius);

		dBodySetMass(this->bodyID, &massStruct);
		dGeomSetBody (this->geomID, this->bodyID);

	} else {
		dGeomSetBody(this->geomID, 0);
	}
	
	return true;
}
