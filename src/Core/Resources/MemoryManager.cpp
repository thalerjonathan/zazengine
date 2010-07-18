/*
 * MemoryManager.cpp
 *
 *  Created on: 29.06.2010
 *      Author: joni
 */

#include "MemoryManager.h"

#include "MemoryPool.h"

MemoryManager::MemoryManager(MemoryPool* parent)
{
	this->parent = parent;
}

MemoryManager::~MemoryManager()
{
	this->parent->returnMemoryManager(this);
}

template<typename T> Handle<T> MemoryManager::instantiate()
{
	// TODO: implement
	Handle<T> hnd;
	hnd.instance = new T();
	hnd.origin = this;
}

template<typename T> Handle<T> MemoryManager::instantiateMulti(long count)
{
	// TODO: implement
	Handle<T> hnd;
	hnd.instance = new T();
	hnd.origin = this;
}

bool MemoryManager::poolInit(char* memory, long size)
{
	return this->initialize(memory, size);
}
