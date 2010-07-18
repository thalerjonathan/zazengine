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
		PlayGroundPhysicsEntity() {};
		virtual ~PlayGroundPhysicsEntity() {};

		const std::string& getID() const { return this->id; };

	private:
		std::string id;
};

#endif /* PLAYGROUNDPHYSICSENTITY_H_ */
