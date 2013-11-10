#ifndef _ZAZENGAMEOBJECT_H_
#define _ZAZENGAMEOBJECT_H_

#include <core/IGameObject.h>

#include "DllExport.h"

class DLL_API ZazenGameObject : public IGameObject
{
	public:
		ZazenGameObject( const std::string& );
		virtual ~ZazenGameObject();

		GameObjectID getID() const { return m_id; };
		const std::string& getName() const { return this->m_name; };
		const std::string& getObjectClass() const { return this->m_objectClass; };

		bool sendEvent( Event& );

		IGameObject* clone() { return 0; };

		bool initialize( TiXmlElement* );

		boost::any getProperty( const std::string& id )  { return this->m_properties[ id ]; };

	private:
		static GameObjectID nextID;

		GameObjectID m_id;
		std::string m_name;
		std::string m_objectClass;

		std::map<std::string, ISubSystemEntity*> m_subSystemEntities;
		std::map<std::string, boost::any> m_properties;
};

#endif /* _ZAZENGAMEOBJECT_H_ */
