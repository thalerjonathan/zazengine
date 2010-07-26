#ifndef RENDERER_H_
#define RENDERER_H_

#include "../Scene/Camera.h"
#include "../Geometry/GeomInstance.h"
#include "../Geometry/GeomSkyBox.h"

class Renderer
{
 public:
	Renderer( Camera& camera, std::string& );
	virtual ~Renderer();

	virtual void renderFrame( GeomInstance* ) = 0;

	void printInfo();

 protected:
	 long long renderedFaces;
	 int renderedInstances;
	 int occludedInstances;
	 int culledInstances;

	 long long frame;

	 Camera& camera;
	 GeomSkyBox* skyBox;

	 static bool geomInstanceDistCmp( GeomInstance* a, GeomInstance* b )
	 {
	 	return a->distance < b->distance;
	 }
};

#endif /*STANDARDTRAVERSAL_H_*/
