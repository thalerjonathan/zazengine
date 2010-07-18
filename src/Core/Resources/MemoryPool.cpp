/*
 * MemoryPool.cpp
 *
 *  Created on: 28.06.2010
 *      Author: joni
 */

#include "MemoryPool.h"

#include "SimpleMemoryManager.h"

#include <stdlib.h>
#include <iostream>

MemoryPool::MemoryPool()
{
	this->freeSize = 0;
	this->memorySize = 0;
	this->memory = 0;
}

MemoryPool::~MemoryPool()
{
	if (this->memory)
	{
		free (this->memory);
	}
}

MemoryManager*
MemoryPool::createMemoryManager(const std::string& id, long size)
{
	MemoryManager* manager = 0;

	if ( "SIMPLE" == id)
	{
		manager = new SimpleMemoryManager(this);
		manager->poolInit(0, size);
	}
	else
	{
		std::cout << "Unsupported MemoryManager " << id << std::endl;
	}

	return manager;
}

void
MemoryPool::returnMemoryManager(MemoryManager* manager)
{
	// TODO: search for usage
}

bool
MemoryPool::initialize(long size)
{
	this->memorySize = 0;

	this->memory = (char*) malloc(size);
	if ( 0 == this->memory)
	{
			// TODO: throw exception
	}

	return true;
}

