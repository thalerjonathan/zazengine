/*
 * MemoryManager.h
 *
 *  Created on: 28.06.2010
 *      Author: joni
 */

#ifndef MEMORYMANAGER_H_
#define MEMORYMANAGER_H_

#include "Handle.h"


	// TODO: create a MemoryManager subclass: SyncMemoryManager which can hold a memorymanager
	// and just delegates calls in a synced way

class MemoryPool;

class MemoryManager
{
	friend class MemoryPool;

	public:
		MemoryManager(MemoryPool* parent);
		virtual ~MemoryManager();

		template<typename T> Handle<T> instantiate();
		template<typename T> Handle<T> instantiateMulti(long count);

		virtual char* malloc(long size) = 0;
		virtual void free(char*) = 0;

	protected:
		virtual bool initialize(char* memory, long size) = 0;

	private:
		MemoryPool* parent;

		bool poolInit(char* memory, long size);

};

#endif /* MEMORYMANAGER_H_ */
