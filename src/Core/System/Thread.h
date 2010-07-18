/*
 * Thread.h
 *
 *  Created on: 18.07.2010
 *      Author: jonathan
 */

#ifndef THREAD_H_
#define THREAD_H_

#ifdef __linux__
	#include <pthread.h>
	#include <errno.h>
	#include <string.h>
	#include <stdexcept>
#endif

#ifdef __MACOSX__
	#include <pthread.h>
	#include <errno.h>
	#include <string.h>
	#include <stdexcept>
#endif

class Thread
{
	public:
		typedef void*(*ThreadFunc)(void*);

		Thread()
		{
		};

		~Thread()
		{
		}

		void start(ThreadFunc func, void* arg)
		{
			#ifdef __linux__
				pthread_attr_t threadAttributes;
				if ( 0 == pthread_attr_init( &threadAttributes ) )
				{
					if ( 0 == pthread_attr_setdetachstate( &threadAttributes, PTHREAD_CREATE_JOINABLE ) )
					{
						if ( 0 != pthread_create( &this->threadHandle, &threadAttributes, func, arg ) )
						{
							throw std::runtime_error( strerror(errno) );
						}
					}
					else
					{
						throw std::runtime_error( strerror(errno) );
					}
				}
				else
				{
					throw std::runtime_error( strerror(errno) );
				}
			#endif

			#ifdef __MACOSX__
				pthread_attr_t threadAttributes;
				if ( 0 == pthread_attr_init( &threadAttributes ) )
				{
					if ( 0 == pthread_attr_setdetachstate( &threadAttributes, PTHREAD_CREATE_JOINABLE ) )
					{
						if ( 0 != pthread_create( &this->threadHandle, &threadAttributes, func, arg ) )
						{
							throw std::runtime_error( strerror(errno) );
						}
					}
					else
					{
						throw std::runtime_error( strerror(errno) );
					}
				}
				else
				{
					throw std::runtime_error( strerror(errno) );
				}
			#endif
		}

		void join()
		{
			#ifdef __linux__
				if ( 0 != pthread_join( this->threadHandle, NULL ) )
				{
					throw std::runtime_error( strerror(errno) );
				}
			#endif

			#ifdef __MACOSX__
				if ( 0 != pthread_join( this->threadHandle, NULL ) )
				{
					throw std::runtime_error( strerror(errno) );
				}
			#endif
		}

	private:
		#ifdef __linux__
			pthread_t threadHandle;
		#endif

		#ifdef __MACOSX__
			pthread_t threadHandle;
		#endif
};
#endif /* THREAD_H_ */
