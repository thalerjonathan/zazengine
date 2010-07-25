/*
 * ZazenGraphics.h
 *
 *  Created on: 06.07.2010
 *      Author: joni
 */

#ifndef ZAZENGRAPHICS_H_
#define ZAZENGRAPHICS_H_

#include "../../../Core/SubSystems/IFaces/IGraphics.h"

#include "ZazenGraphicsEntity.h"

#include "Renderer/Scene.h"
#include "Renderer/Camera.h"

#include <SDL/SDL.h>

class ZazenGraphics : public IGraphics
{
	public:
		ZazenGraphics();
		virtual ~ZazenGraphics();

		const std::string& getID() const { return this->id; };
		const std::string& getType() const { return this->type; };

		bool isAsync() const { return false; };

		bool initialize( TiXmlElement* );
		bool shutdown();

		bool start();
		bool stop();
		bool pause();

		bool process( double );
		bool finalizeProcess();

		bool sendEvent( const Event& e );

		ZazenGraphicsEntity* createEntity( TiXmlElement* );

	private:
		std::string id;
		std::string type;
	
		SDL_Surface* drawContext;
		
		Camera* camera;
		Scene* activeScene;
	
		bool loadGeomClasses( TiXmlElement* );
};

#endif /* ZAZENGRAPHICS_H_ */
