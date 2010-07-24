/*
 * PlaygroundGraphics.h
 *
 *  Created on: 06.07.2010
 *      Author: joni
 */

#ifndef PLAYGROUNDGRAPHICS_H_
#define PLAYGROUNDGRAPHICS_H_

#include "../../IFaces/IGraphics.h"

#include "PlayGroundGraphicsEntity.h"

#include "Scene.h"
#include "graphic/Camera.h"

#include <SDL/SDL.h>

class PlayGroundGraphics : public IGraphics
{
	public:
		PlayGroundGraphics();
		virtual ~PlayGroundGraphics();

		const std::string& getID() const { return this->id; };
		const std::string& getType() const { return this->type; };

		bool isAsync() const { return false; };

		bool initialize();
		bool shutdown();

		bool start();
		bool stop();
		bool pause();

		bool process(double);
		bool finalizeProcess();

		bool sendEvent(const Event& e);

		PlayGroundGraphicsEntity* createEntity( TiXmlElement* );

	private:
		std::string id;
		std::string type;
	
		SDL_Surface* drawContext;
		
		Camera* camera;
		Scene* activeScene;
		std::vector<Scene*> scenes;
	
};

#endif /* PLAYGROUNDRENDERER_H_ */
