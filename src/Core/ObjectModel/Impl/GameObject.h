/*
 * EventContext.h
 *
 *  Created on: 27.06.2010
 *      Author: joni
 */

#ifndef GAMEOBJECT_H_
#define GAMEOBJECT_H_

#include "../IFaces/IGameObject.h"

#include "../../EventSystem/Event.h"
#include "../../EventSystem/IEventListener.h"

class GameObject : public IGameObject
{
	public:
		GameObject(const std::string&);
		virtual ~GameObject();

		GameObjectID getID() const { return id; };
		const std::string& getName() const { return this->name; };

		bool sendEvent(const Event&);

	private:
		static GameObjectID nextID;

		GameObjectID id;
		std::string name;
};

#endif /* EVENTCONTEXT_H_ */
