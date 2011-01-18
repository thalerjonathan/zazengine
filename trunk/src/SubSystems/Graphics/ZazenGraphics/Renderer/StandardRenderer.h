#ifndef STDRENDERER_H_
#define STDRENDERER_H_

#include "Renderer.h"

#include <list>

class StandardRenderer : public Renderer
{
 public:
	StandardRenderer(Camera& camera, std::string&);
	virtual ~StandardRenderer();

	virtual bool initialize();
	virtual bool shutdown();

	 // renders this list of geominstances which must be in front-to-back order
	 void renderFrame(GeomInstance*);

};

#endif /*STDRENDERER_H_*/
