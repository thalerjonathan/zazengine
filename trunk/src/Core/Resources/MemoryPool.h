/*
 * MemoryPool.h
 *
 *  Created on: 28.06.2010
 *      Author: joni
 */

#ifndef MEMORYPOOL_H_
#define MEMORYPOOL_H_

#include "MemoryManager.h"

#include <string>

class MemoryPool
{
	public:
		MemoryPool();
		~MemoryPool();

		bool initialize(long size);

		long getTotalSpace() const { return this->memorySize; };
		long getFreeSpace() { return this->freeSize; };

		MemoryManager* createMemoryManager(const std::string& id, long size);
		void returnMemoryManager(MemoryManager*);

	private:
		char* memory;
		long memorySize;

		long freeSize;
};

#endif /* MEMORYPOOL_H_ */
