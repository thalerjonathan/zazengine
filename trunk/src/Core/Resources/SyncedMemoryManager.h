/*
 * SyncMemoryManager.h
 *
 *  Created on: 30.06.2010
 *      Author: joni
 */

#ifndef SYNCEDMEMORYMANAGER_H_
#define SYNCEDMEMORYMANAGER_H_

#include "MemoryManager.h"

#include <semaphore.h>

class SyncedMemoryManager : public MemoryManager
{
	public:
		SyncedMemoryManager(MemoryManager* manager);
		virtual ~SyncedMemoryManager();

		char* malloc(long size);
		void free(char* data);

	private:
		sem_t protectionSem;

		MemoryManager* wrapedManager;
};


#endif /* SYNCMEMORYMANAGER_H_ */
