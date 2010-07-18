/*
 * SimpleMemoryManager.h
 *
 *  Created on: 29.06.2010
 *      Author: joni
 */

#ifndef SIMPLEMEMORYMANAGER_H_
#define SIMPLEMEMORYMANAGER_H_

#include "MemoryManager.h"

class SimpleMemoryManager : public MemoryManager
{
	public:
		SimpleMemoryManager(MemoryPool* parent);
		virtual ~SimpleMemoryManager();

		char* malloc(long size);
		void free(char* data);

	protected:
		bool initialize(char* memory, long size);
};

#endif /* SIMPLEMEMORYMANAGER_H_ */
