/*
 * IPhysicsEntity.h
 *
 *  Created on: 15.07.2010
 *      Author: joni
 */

#ifndef IPHYSICSENTITY_H_
#define IPHYSICSENTITY_H_

#include "ISubSystemEntity.h"

class IPhysicsEntity : public ISubSystemEntity
{
	public:
		virtual ~IPhysicsEntity() {};

		virtual const double* getPos() const = 0;
		virtual const double* getRot() const = 0;
		virtual const double* getVel() const = 0;
};

#endif /* IPHYSICSENTITY_H_ */
