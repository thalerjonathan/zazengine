/*
 * PlayGroundPhysicsEntity.h
 *
 *  Created on: 08.07.2010
 *      Author: joni
 */

#ifndef ODEPHYSICSENTITY_H_
#define ODEPHYSICSENTITY_H_

#include "../../../Core/SubSystems/IFaces/IPhysicsEntity.h"

class ODEPhysicsEntity : public IPhysicsEntity
{
	public:
		ODEPhysicsEntity() : type ("physics") {};
		virtual ~ODEPhysicsEntity() {};

		const std::string& getType() const { return this->type; };

	private:
		std::string type;
};

#endif /* PLAYGROUNDPHYSICSENTITY_H_ */
