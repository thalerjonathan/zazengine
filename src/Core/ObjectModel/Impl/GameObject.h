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
		GameObject();
		virtual ~GameObject();

		GameObjectID getID() const { return id; };
		const std::string& getName() const { return this->name; };

		bool sendEvent(const Event&);

		IGameObject* clone() { return 0; };

		bool initialize( TiXmlElement* );

	private:
		static GameObjectID nextID;

		GameObjectID id;
		std::string name;

		std::map<std::string, ISubSystemEntity*> subSystemEntities;
};

#endif /* EVENTCONTEXT_H_ */
