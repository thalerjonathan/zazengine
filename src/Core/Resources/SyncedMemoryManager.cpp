/*
 * SyncedMemoryManager.cpp
 *
 *  Created on: 30.06.2010
 *      Author: joni
 */

#include "SyncedMemoryManager.h"

SyncedMemoryManager::SyncedMemoryManager(MemoryManager* manager)
	: MemoryManager(0)
{
	this->wrapedManager = manager;
}

SyncedMemoryManager::~SyncedMemoryManager()
{

}

char* SyncedMemoryManager::malloc(long size)
{
	return this->wrapedManager->malloc(size);
}

void SyncedMemoryManager::free(char* data)
{
	this->wrapedManager->free(data);
}
