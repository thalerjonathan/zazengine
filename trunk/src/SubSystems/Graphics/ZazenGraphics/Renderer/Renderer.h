#ifndef RENDERER_H_
#define RENDERER_H_

#include "Camera.h"
#include "Geominstance.h"
#include "geom/GeomSkyBox.h"

class Renderer
{
 public:
	Renderer(Camera& camera, std::string&);
	virtual ~Renderer();

	virtual void renderFrame(GeomInstance*) = 0;

	void renderText( const std::string&, int x, int y );

	void printInfo();

 protected:
	 long long renderedFaces;
	 int renderedInstances;
	 int occludedInstances;
	 int culledInstances;

	 long long frame;

	 Camera& camera;
	 GeomSkyBox* skyBox;

	 static bool geomInstanceDistCmp(GeomInstance* a, GeomInstance* b)
	 {
	 	return a->distance < b->distance;
	 }
};

#endif /*STANDARDTRAVERSAL_H_*/
