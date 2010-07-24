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
		PlayGroundAIEntity() : type("ai") {};
		virtual ~PlayGroundAIEntity() {};

		const std::string& getType() const { return this->type; };

	private:
		std::string type;
};

#endif /* PLAYGROUNDAIENTITY_H_ */
