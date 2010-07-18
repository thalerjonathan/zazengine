/*
 * IGameObject.h
 *
 *  Created on: 07.07.2010
 *      Author: joni
 */

#ifndef IGAMEOBJECT_H_
#define IGAMEOBJECT_H_

#include "../../EventSystem/IEventListener.h"

class ISubSystemEntity;

class IGameObject : public IEventListener
{
	public:
		typedef long long GameObjectID;

		virtual ~IGameObject() {};

		virtual GameObjectID getID() const = 0;
		virtual const std::string& getName() const = 0;

		virtual IGameObject* clone() = 0;

	private:
		std::map<std::string, ISubSystemEntity*> subSystemEntities;
};

#endif /* IGAMEOBJECT_H_ */
