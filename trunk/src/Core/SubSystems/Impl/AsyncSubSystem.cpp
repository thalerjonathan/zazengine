/*
 * AsyncSubSystem.cpp
 *
 *  Created on: 27.06.2010
 *      Author: joni
 */

#include "AsyncSubSystem.h"

#include "../../Core.h"

#include <sys/time.h>
#include <iostream>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

using namespace std;

AsyncSubSystem::AsyncSubSystem( const string& id )
	: id (id)
{
	this->queueSem = 0;
	this->threadHandle = 0;
}

AsyncSubSystem::~AsyncSubSystem()
{

}

bool
AsyncSubSystem::start()
{
		cout << "Starting Async AsyncSubSystem " << this->id << "..." << endl;

		this->queueSem = sem_open( this->id.c_str(), O_CREAT, 777, 1 );
		if ( SEM_FAILED == this->queueSem )
		{
			cout << "sem_init failed in Async AsyncSubSystem " << this->id << " with reason " << strerror(errno) << " - exit" << endl;

			return false;
		}

		this->runThread = true;

		pthread_attr_t threadAttributes;
		if ( 0 == pthread_attr_init(&threadAttributes))
		{
			if ( 0 == pthread_attr_setdetachstate(&threadAttributes, PTHREAD_CREATE_JOINABLE))
			{
				if ( 0 != pthread_create(&this->threadHandle, &threadAttributes, AsyncSubSystem::threadProcessor, this))
				{
					cout << "AsyncSubSystem::start pthread_create ERR!" << endl;
					return false;
				}
			}
			else
			{
				cout << "AsyncSubSystem::start pthread_attr_setdetachstate ERR!" << endl;
				return false;
			}
		}
		else
		{
			cout << "AsyncSubSystem::start pthread_attr_init ERR!" << endl;
			return false;
		}

	return true;
}

bool
AsyncSubSystem::stop()
{
		cout << "Stopping Async AsyncSubSystem " << this->id << "..." << endl;

		this->runThread = false;

		pthread_join( this->threadHandle, NULL );
		sem_close( this->queueSem );
		sem_unlink ( this->id.c_str() );

		return true;
}

bool
AsyncSubSystem::pause()
{
	return true;
}

bool
AsyncSubSystem::sendEvent(const Event& e)
{
		sem_wait( this->queueSem );
		this->eventQueue.push_back( e );
		sem_post( this->queueSem );

	return true;
}

bool
AsyncSubSystem::process()
{
	this->preProcess();

	std::list<Event> backInsertQueue;

	sem_wait( this->queueSem );
	std::list<Event> eventQueueCopy(this->eventQueue);
	this->eventQueue.clear();
	sem_post( this->queueSem );

	std::list<Event>::iterator it = eventQueueCopy.begin();
	while (it != eventQueueCopy.end())
	{
		Event& e = *it++;

		// not yet finished => push back into async
		if ( false == this->processEvent( e ) )
		{
			backInsertQueue.push_back( e );
		}

	}

	sem_wait( this->queueSem );

	it = backInsertQueue.begin();
	while (it != backInsertQueue.end())
	{
		this->eventQueue.push_back(*it++);
	}

	sem_post( this->queueSem );

	this->postProcess();

	return true;
}


void*
AsyncSubSystem::threadProcessor(void* args)
{
	AsyncSubSystem* instance = (AsyncSubSystem*) args;

	cout << "Async AsyncSubSystem " << instance->id << " is up and processing" << endl;

	while ( instance->runThread )
	{
		instance->process();
	}

	return 0;
}
