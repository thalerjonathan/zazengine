/*
 * ZazenGraphicsEntity.h
 *
 *  Created on: 08.07.2010
 *      Author: joni
 */

#ifndef ZAZENGRAPHICSENTITY_H_
#define ZAZENGRAPHICSENTITY_H_

#include "../../../Core/SubSystems/IFaces/IGraphicsEntity.h"

#include "Scene/Scene.h"

class ZazenGraphicsEntity : public IGraphicsEntity
{
	public:
		friend class ZazenGraphics;

		ZazenGraphicsEntity( IGameObject* p );
		virtual ~ZazenGraphicsEntity();

		const std::string& getType() const { return this->type; };

		virtual void setOrientation( const float* pos, const float* rot);

		virtual bool sendEvent( Event& e );

	private:
		std::string type;

		Scene::InstanceDefinition* instance;
};

#endif /* ZAZENGRAPHICSENTITY_H_ */
