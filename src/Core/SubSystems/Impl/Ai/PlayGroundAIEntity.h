/*
 * PlayGroundAIEntity.h
 *
 *  Created on: 08.07.2010
 *      Author: joni
 */

#ifndef PLAYGROUNDAIENTITY_H_
#define PLAYGROUNDAIENTITY_H_

#include "../../IFaces/IAIEntity.h"

class PlayGroundAIEntity : public IAIEntity
{
	public:
		PlayGroundAIEntity() {};
		virtual ~PlayGroundAIEntity() {};

		const std::string& getID() const { return this->id; };

	private:
		std::string id;
};

#endif /* PLAYGROUNDAIENTITY_H_ */
