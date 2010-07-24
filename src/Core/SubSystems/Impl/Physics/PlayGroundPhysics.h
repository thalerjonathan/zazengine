/*
 * PlaygroundPhysics.h
 *
 *  Created on: 06.07.2010
 *      Author: joni
 */

#ifndef PLAYGROUNDPHYSICS_H_
#define PLAYGROUNDPHYSICS_H_

#include "../../IFaces/IPhysics.h"

#include "PlayGroundPhysicsEntity.h"

#include "../../../System/Semaphore.h"
#include "../../../System/Thread.h"

#include <ode/ode.h>

class PlayGroundPhysics : public IPhysics
{
	public:
		PlayGroundPhysics();
		virtual ~PlayGroundPhysics();

		const std::string& getID() const { return this->id; };
		const std::string& getType() const { return this->type; };

		bool isAsync() const { return true; };

		bool start();
		bool stop();
		bool pause();

		bool initialize();
		bool shutdown();

		bool process(double);
		bool finalizeProcess();

		bool sendEvent(const Event& e);

		PlayGroundPhysicsEntity* createEntity( TiXmlElement* );

	private:
		std::string id;
		std::string type;

		bool runThread;
		bool doProcessing;

		Semaphore* sem;
		Thread* thread;

		dWorldID worldID;
		dSpaceID spaceID;
		dJointGroupID contactGroupID;

		void processInternal();

		static void collisionCallback(void *data, dGeomID o1, dGeomID o2);

		static void* threadFunc(void*);
};

#endif /* PLAYGROUNDPHYSICS_H_ */
