/*
 * PlayGroundPhysicsEntity.h
 *
 *  Created on: 08.07.2010
 *      Author: joni
 */

#ifndef ODEPHYSICSENTITY_H_
#define ODEPHYSICSENTITY_H_

#include "../../../Core/SubSystems/IFaces/IPhysicsEntity.h"

#include "types/PhysicType.h"

class ODEPhysicsEntity : public IPhysicsEntity
{
	public:
		friend class ODEPhysics;

		ODEPhysicsEntity( IGameObject* p );
		virtual ~ODEPhysicsEntity();

		const std::string& getType() const { return this->entityType; };

		const bool isStatic() const { return this->physicType->isStatic(); };

		const float* getPos() const { return dBodyGetPosition( this->physicType->getBodyID() ); };
		const float* getRot() const { return dBodyGetRotation( this->physicType->getBodyID() ); };
		const float* getVel() const { return dBodyGetLinearVel( this->physicType->getBodyID() ); };

		virtual bool sendEvent(const Event& e);

	private:
		std::string entityType;

		PhysicType* physicType;

};

#endif /* PLAYGROUNDPHYSICSENTITY_H_ */
