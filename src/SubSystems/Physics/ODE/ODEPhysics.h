/*
 * ODEPhysics.h
 *
 *  Created on: 06.07.2010
 *      Author: joni
 */

#ifndef ODEPHYSICS_H_
#define ODEPHYSICS_H_

#include "../../../Core/SubSystems/IFaces/IPhysics.h"

#include "ODEPhysicsEntity.h"

#include "../../../Core/System/Semaphore.h"
#include "../../../Core/System/Thread.h"

#include <ode/ode.h>

class ODEPhysics : public IPhysics
{
	public:
		ODEPhysics();
		virtual ~ODEPhysics();

		const std::string& getID() const { return this->id; };
		const std::string& getType() const { return this->type; };

		bool isAsync() const { return true; };

		bool start();
		bool stop();
		bool pause();

		bool initialize( TiXmlElement* );
		bool shutdown();

		bool process( double );
		bool finalizeProcess();

		bool sendEvent( const Event& e );

		ODEPhysicsEntity* createEntity( TiXmlElement* );

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
