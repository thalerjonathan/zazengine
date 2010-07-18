/*
 * SimpleMemoryManager.cpp
 *
 *  Created on: 29.06.2010
 *      Author: joni
 */

#include "SimpleMemoryManager.h"

SimpleMemoryManager::SimpleMemoryManager(MemoryPool* parent)
	: MemoryManager(parent)
{

}

SimpleMemoryManager::~SimpleMemoryManager()
{

}

char*
SimpleMemoryManager::malloc(long size)
{
	return 0;
}

void
SimpleMemoryManager::free(char* data)
{

}

bool
SimpleMemoryManager::initialize(char* memory, long size)
{
	return true;
}
