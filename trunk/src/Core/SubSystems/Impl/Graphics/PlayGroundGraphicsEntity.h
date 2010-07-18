/*
 * PlayGroundGraphicsEntity.h
 *
 *  Created on: 08.07.2010
 *      Author: joni
 */

#ifndef PLAYGROUNDGRAPHICSENTITY_H_
#define PLAYGROUNDGRAPHICSENTITY_H_

#include "../../IFaces/IGraphicsEntity.h"
#include "../../IFaces/IConsumer.h"

class PlayGroundGraphicsEntity : public IGraphicsEntity, public IConsumer
{
	public:
		PlayGroundGraphicsEntity();
		virtual ~PlayGroundGraphicsEntity();

		const std::string& getType() const { return this->type; };

		void consume( ISubSystemEntity* );

	private:
		std::string type;
};

#endif /* PLAYGROUNDGRAPHICSENTITY_H_ */
