/*
 * Semaphore.h
 *
 *  Created on: 18.07.2010
 *      Author: jonathan
 */

#ifndef SEMAPHORE_H_
#define SEMAPHORE_H_

#ifdef __linux__
	#include <semaphore.h>
	#include <errno.h>
	#include <string.h>
	#include <stdexcept>
#endif

#ifdef __MACOSX__
	#include <semaphore.h>
	#include <errno.h>
	#include <string.h>
	#include <stdexcept>
#endif

class Semaphore
{
	public:
		Semaphore()
		{
			#ifdef __linux__
				if ( 0 != sem_init( &this->sem, 0, 1 ) )
				{
					throw std::runtime_error( strerror(errno) );
				}
			#endif

			#ifdef __MACOSX__
				this->threadSem = sem_open( this->id.c_str(), O_CREAT, 777, 1 );
				if ( SEM_FAILED == this->threadSem )
				{
					throw std::runtime_error( strerror(errno) );
				}
			#endif
		}

		~Semaphore()
		{
			#ifdef __linux__
				sem_close( &this->sem );
			#endif

			#ifdef __MACOSX__
				sem_close( &this->sem );
				sem_unlink ( this->id.c_str() );
			#endif
		}

		void grab()
		{
			#ifdef __linux__
				if ( 0 != sem_wait( &this->sem ) )
					throw std::runtime_error( strerror(errno) );
			#endif

			#ifdef __MACOSX__
				if ( 0 != sem_wait( &this->sem ) )
					throw std::runtime_error( strerror(errno) );
			#endif
		}

		void tryGrab()
		{
			#ifdef __linux__
				if ( 0 != sem_trywait( &this->sem ) )
					throw std::runtime_error( strerror(errno) );
			#endif

			#ifdef __MACOSX__
				if ( 0 != sem_trywait( &this->sem ) )
					throw std::runtime_error( strerror(errno) );
			#endif
		}

		void release()
		{
			#ifdef __linux__
				if ( 0 != sem_post( &this->sem ) )
					throw std::runtime_error( strerror(errno) );
			#endif

			#ifdef __MACOSX__
				if ( 0 != sem_post( &this->sem ) )
					throw std::runtime_error( strerror(errno) );
			#endif
		}

	private:

#ifdef __linux__
		sem_t sem;
#endif

#ifdef __MACOSX__
		sem_t sem;
#endif

};

#endif /* SEMAPHORE_H_ */
