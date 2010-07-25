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

		ODEPhysicsEntity() : type ("physics") { this->physicType = 0; };
		virtual ~ODEPhysicsEntity() {};

		const std::string& getType() const { return this->type; };

		const double* getPos() const { return pos; };
		const double* getRot() const { return rot; };
		const double* getVel() const { return vel; };

	private:
		std::string type;

		double pos[3];
		double rot[12];
		double vel[3];

		PhysicType* physicType;

};

#endif /* PLAYGROUNDPHYSICSENTITY_H_ */
