/*
 * ISubSystem.h
 *
 *  Created on: 07.07.2010
 *      Author: joni
 */

#ifndef ISUBSYSTEM_H_
#define ISUBSYSTEM_H_

#include "ISubSystemEntity.h"
#include "../../EventSystem/IEventListener.h"

#include "../../Utils/XML/tinyxml.h"

#include <string>

class ISubSystem : public IEventListener
{
	public:
		virtual ~ISubSystem() {};

		virtual const std::string& getID() const = 0;
		virtual const std::string& getType() const = 0;

		virtual bool isAsync() const = 0;

		virtual bool start() = 0;
		virtual bool stop() = 0;
		virtual bool pause() = 0;

		virtual bool initialize() = 0;
		virtual bool shutdown() = 0;

		virtual bool process(double) = 0;		// runs simulations/calculations for this frame: in this step all calculations are done either synced or threaded
		virtual bool finalizeProcess() = 0;		// finishes processing and must block until threads are finished: in this step all calculations are finished and maybe exchanged through consumer/producer

		virtual ISubSystemEntity* createEntity( TiXmlElement* ) = 0;

};
#endif /* ISUBSYSTEM_H_ */
