/*
 * Handle.h
 *
 *  Created on: 30.06.2010
 *      Author: joni
 */

#ifndef HANDLE_H_
#define HANDLE_H_

class MemoryManager;

template <typename T> class Handle
{
	friend class MemoryManager;

	public:
		T* instance;

		T operator*() { return *instance; };
		T *operator->() { return *instance; };
		T* &operator[](int i) { return instance[i]; };

	private:
		MemoryManager* origin;

};

#endif /* HANDLE_H_ */
