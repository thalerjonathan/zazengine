/*
 * PlayGroundPhysicsEntity.h
 *
 *  Created on: 08.07.2010
 *      Author: joni
 */

#ifndef PLAYGROUNDPHYSICSENTITY_H_
#define PLAYGROUNDPHYSICSENTITY_H_

#include "../../IFaces/IPhysicsEntity.h"

class PlayGroundPhysicsEntity : public IPhysicsEntity
{
	public:
		PlayGroundPhysicsEntity() : type ("physics") {};
		virtual ~PlayGroundPhysicsEntity() {};

		const std::string& getType() const { return this->type; };

	private:
		std::string type;
};

#endif /* PLAYGROUNDPHYSICSENTITY_H_ */
