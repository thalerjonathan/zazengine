#ifndef STDRENDERER_H_
#define STDRENDERER_H_

#include "Renderer.h"

#include <list>

class StandardRenderer : public Renderer
{
 public:
	StandardRenderer(Camera& camera, std::string&);
	virtual ~StandardRenderer();

	 // renders this list of geominstances which must be in front-to-back order
	 void renderFrame(GeomInstance*);

 private:
	 bool parentIntersectingFrustum;

	 std::list<GeomInstance*> instancesQueue;
	 std::list<GeomInstance*> renderQueue;

	 void processRenderQueue();
	 void traverseInstance(GeomInstance*);
	 void processInstance(GeomInstance*);
};

#endif /*STDRENDERER_H_*/
