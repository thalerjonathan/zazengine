#ifndef _ISUBSYSTEM_ENTITY_H_
#define _ISUBSYSTEM_ENTITY_H_

#include <list>

#include "IGameObject.h"

class ISubSystemEntity : public IEventListener
{
	public:
		ISubSystemEntity( IGameObject* parent ) : parent( parent ) {};
		virtual ~ISubSystemEntity() {};

		IGameObject* getParent() { return this->parent; };

		virtual const std::string& getType() const = 0;

		// to be overridden
		virtual void update() {};

	protected:
		std::list<Event> queuedEvents;

	private:
		IGameObject* parent;
};

#endif /* _ISUBSYSTEM_ENTITY_H_ */
