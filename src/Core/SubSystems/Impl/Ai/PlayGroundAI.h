/*
 * PlaygroundAI.h
 *
 *  Created on: 06.07.2010
 *      Author: joni
 */

#ifndef PLAYGROUNDAI_H_
#define PLAYGROUNDAI_H_

#include "../../IFaces/IAi.h"

#include "PlayGroundAIEntity.h"

class PlayGroundAI : public IAi
{
	public:
		PlayGroundAI();
		virtual ~PlayGroundAI();

		const std::string& getID() const { return this->id; };
		const std::string& getType() const { return this->type; };

		bool isAsync() const { return false; };

		bool start();
		bool stop();
		bool pause();

		bool initialize();
		bool shutdown();

		bool process(double);
		bool finalizeProcess();

		bool sendEvent(const Event& e);

		PlayGroundAIEntity* createEntity();

	private:
		std::string id;
		std::string type;

};

#endif /* PLAYGROUNDAI_H_ */
