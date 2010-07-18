/*
 * EventProcessor.h
 *
 *  Created on: 27.06.2010
 *      Author: joni
 */

#ifndef ASYNCSUBSYSTEM_H_
#define ASYNCSUBSYSTEM_H_

#include "../IFaces/ISubSystem.h"

#include "../../EventSystem/Event.h"
#include "../../EventSystem/IEventListener.h"

#include <pthread.h>
#include <semaphore.h>
#include <list>

class AsyncSubSystem : public ISubSystem
{
	public:
		AsyncSubSystem( const std::string& );
		virtual ~AsyncSubSystem();

		const std::string& getID() const { return this->id; };
		bool isAsync() const { return true; };

		bool sendEvent(const Event&);

		bool start();
		bool stop();
		bool pause();

		bool process();

	protected:
		virtual bool processEvent( const Event& ) = 0;
		virtual bool preProcess() = 0;
		virtual bool postProcess() = 0;

	private:
		std::string id;

		bool runThread;

		sem_t* queueSem;
		pthread_t threadHandle;
		std::list<Event> eventQueue;

		static void* threadProcessor(void*);

};

#endif /* ASYNCSUBSYSTEM_H_ */
