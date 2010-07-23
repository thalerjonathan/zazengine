/*
 * IPhysics.h
 *
 *  Created on: 05.07.2010
 *      Author: joni
 */

#ifndef IPHYSICS_H_
#define IPHYSICS_H_

#include "ISubSystem.h"
#include "IPhysicsEntity.h"

class IPhysics : public ISubSystem
{
	public:
		virtual ~IPhysics() {};

		virtual IPhysicsEntity* createEntity() = 0;

};

#endif /* IPHYSICS_H_ */