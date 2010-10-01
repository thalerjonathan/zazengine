/*
 * ISubSystemEntity.h
 *
 *  Created on: 07.07.2010
 *      Author: joni
 */

#ifndef ISUBSYSTEMENTITY_H_
#define ISUBSYSTEMENTITY_H_

#include <string>
#include <list>

#include "../../ObjectModel/IFaces/IGameObject.h"

class ISubSystemEntity : public IEventListener
{
	public:
		ISubSystemEntity( IGameObject* parent ) : parent( parent ) {};
		virtual ~ISubSystemEntity() {};

		IGameObject* getParent() { return this->parent; };

		virtual const std::string& getType() const = 0;

	protected:
		std::list<Event> queuedEvents;

	private:
		IGameObject* parent;


};

#endif /* ISUBSYSTEMENTITY_H_ */
